#ifndef UTIL_H
#define UTIL_H

#include <SDL.h>

void free_surfaces();
SDL_Surface* load_surface(SDL_Surface* screen, const char* path,
  bool optimize = true);
void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
void draw_border(SDL_Surface* surface, SDL_Rect rect);
void draw_sample(SDL_Surface* screen, Uint8* abuf, int len, Uint32 color);

void apply_delay(int chan, void* abuf, int len, void* data);
void apply_delay_post(int chan, void* data);

#endif  // UTIL_H