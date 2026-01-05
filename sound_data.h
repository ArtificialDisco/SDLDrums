#ifndef SOUND_DATA_H
#define SOUND_DATA_H

#include <SDL.h>
#ifdef __linux__ 
#include <SDL2/SDL_mixer.h>
#elif _WIN32
#include <SDL_mixer.h>
#endif

class SoundData {
 public:
  SoundData() = default;
  ~SoundData();

  void PlaySample(int n);
  void PlaySampleFromKeycode(SDL_Keycode key);
  bool LoadSamples(const char** files);

 private:
   Mix_Chunk* samples_[9];
};

#endif  // SOUND_DATA_H
