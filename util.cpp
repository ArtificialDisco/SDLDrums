#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <stdio.h>

#include "util.h"
#include "sound_data.h"

// Keep track of all surfaces so we can free them correctly
// Total surfaces are currently 64. Update this when needed.
SDL_Surface* all_surfaces[80];
int total_surfaces = 0;

void free_surfaces() {
  printf("Freeing %i surfaces...\n", total_surfaces);
  for (int i = 0; i < total_surfaces; i++) {
    SDL_FreeSurface(all_surfaces[i]);
  }
}

SDL_Surface* load_surface(SDL_Surface* screen, const char* path,
  bool optimize) {
  SDL_Surface* optimizedSurface = NULL;

  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL) {
    printf("Unable to load image %s!\nSDL_image Error: %s\n", path, IMG_GetError());
  }
  else {
    if (!optimize) {
      all_surfaces[total_surfaces++] = optimizedSurface;
      return loadedSurface;
    }
    optimizedSurface = SDL_ConvertSurface(loadedSurface, screen->format, 0);
    if (optimizedSurface == NULL) {
      printf("Unable to optimize image %s!\nSDL Error: %s\n", path, SDL_GetError());
    }

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  all_surfaces[total_surfaces++] = optimizedSurface;
  return optimizedSurface;
}

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

  if (x > surface->w || x < 0) return;
  if (y > surface->h || y < 0) return;

  switch (bpp) {
  case 1:
    *p = pixel;
    break;
  case 2:
    *(Uint16*)p = pixel;
    break;
  case 3:
    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    }
    else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *(Uint32*)p = pixel;
    break;
  }
}

void draw_border(SDL_Surface* surface, SDL_Rect rect)
{
  SDL_Rect top, bottom, left, right;
  Uint32 yellow = SDL_MapRGB(surface->format, 0xff, 0xb8, 0x2a);

  top.x = rect.x - 1, top.y = rect.y - 1;
  top.w = rect.w + 2, top.h = 2;

  left.x = rect.x - 1, left.y = rect.y - 1;
  left.w = 2, left.h = rect.h + 2;

  bottom.x = rect.x - 1, bottom.y = rect.y + rect.h + 1;
  bottom.w = rect.w + 2, bottom.h = 1;

  right.x = rect.x + rect.w, right.y = rect.y;
  right.w = 1, right.h = rect.h + 1;

  SDL_FillRect(surface, &top, yellow);
  SDL_FillRect(surface, &left, yellow);
  SDL_FillRect(surface, &bottom, yellow);
  SDL_FillRect(surface, &right, yellow);
}


void draw_sample(SDL_Surface* screen, Uint8* abuf, int len, Uint32 color) {
  Uint16 format;
  int channels, incr;
  Sint16* buffer16;
  Sint32* buffer32;

  Mix_QuerySpec(NULL, &format, &channels);
  incr = (format & 0xFF) * channels;

  unsigned long int i;
  unsigned long int length = len / 2;
  switch (format & 0xFF) {
  case 16:
    buffer16 = reinterpret_cast<Sint16*>(abuf);
    for (i = 0; i < length; i += 2) {
      Sint16 current_l = *(buffer16);
      Sint16 current_r = *(buffer16 + 1);
      int x = i * screen->w / length;
      int y = screen->h / 2 + ((current_l + current_r) / 2 * (screen->h)) / (2 << 16);
      putpixel(screen, i * screen->w / length,
        screen->h / 2 +
        ((current_l + current_r) / 2 * (screen->h)) / (2 << 16),
        color);
      buffer16 += 2;
    }
    break;
  case 32:
    buffer32 = reinterpret_cast<Sint32*>(abuf);
    for (i = 0; i < length; i += 4) {
      Sint32 current_l = *(buffer32);
      Sint32 current_r = *(buffer32 + 1);
      putpixel(screen, i * screen->w / length,
        screen->h / 2ll + 100ll +
        ((current_l) * (screen->h / 2ll)) / (2ll << 32ll),
        color);
      putpixel(screen, i * screen->w / length,
        screen->h / 2ll - 100ll +
        ((current_r) * (screen->h / 2ll)) / (2ll << 32ll),
        color);
      buffer32 += 2;
    }
    break;
  }
}

Uint8 delayBuffer[88200];
int db_idx = 0;
int delay_init = 0;
int milliseconds = 500;

void apply_delay(int chan, void* abuf, int len, void* data) {
  printf("apply_delay\n");
  if (delay_init == 0) {
    for (int i = 0; i < 88200; i++) {
      delayBuffer[i] = 0;
    }
    delay_init = 1;
  }

  Sint16* delayBuffer16 = (Sint16*)delayBuffer;
  Sint16* abuf16 = (Sint16*)abuf;
  int nsamples = SampleRate / 1000.0 * milliseconds;

  for (int i = 0; i < len / 2; i += 2) {
    delayBuffer16[db_idx] = abuf16[i];
    delayBuffer16[db_idx + 1] = abuf16[i + 1];

    abuf16[i] = abuf16[i + 1] * 0.5 +
                delayBuffer16[((db_idx + 1 - nsamples) + SampleRate) % SampleRate] * 0.5;
    abuf16[i + 1] = abuf16[i] * 0.5 +
                    delayBuffer16[((db_idx - nsamples) + SampleRate) % SampleRate] * 0.5;

    db_idx += 2;

    if (db_idx == 44100) {
      printf("Reset Buffer\n");
      db_idx = 0;
    }
  }
  printf("%i\n", db_idx);
}

void apply_delay_post(int chan, void* data) {

}