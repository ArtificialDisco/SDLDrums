#include "sound_data.h"

#include <stdio.h>

SoundData::~SoundData() {
  for (unsigned i = 0; i < 9; i++) {
    Mix_FreeChunk(samples_[i]);
  }
}

bool SoundData::LoadSamples(const char** files) {
  for (unsigned i = 0; i < 9; i++) {
    samples_[i] = Mix_LoadWAV(files[i]);
    if (samples_[i] == NULL) {
      printf("Failed to load sound %s: %s\n", files[i], Mix_GetError());
      return false;
    }
  }
  return true;
}

void SoundData::PlaySample(int n) {
  Mix_PlayChannel(-1, samples_[n], 0);
}

void SoundData::PlaySampleFromKeycode(SDL_Keycode key) {
  int n = 0;
  switch (key) {
    case SDLK_z: n = 0; break;
    case SDLK_x: n = 1; break;
    case SDLK_c: n = 2; break;
    case SDLK_a: n = 3; break;
    case SDLK_s: n = 4; break;
    case SDLK_d: n = 5; break;
    case SDLK_q: n = 6; break;
    case SDLK_w: n = 7; break;
    case SDLK_e: n = 8; break;
  }
  PlaySample(n);
}
