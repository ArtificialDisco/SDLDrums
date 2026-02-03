#ifndef SOUND_DATA_H
#define SOUND_DATA_H

#include <SDL.h>
#ifdef __linux__ 
#include <SDL2/SDL_mixer.h>
#elif _WIN32
#include <SDL_mixer.h>
#endif

#include <memory>

const int SampleRate = 44100;

class DelayEffect {
 public:
  DelayEffect();
  ~DelayEffect();
  void AddToBuffer(Mix_Chunk* chunk);
  void ApplyDelay(Uint8* stream, int len);
  void AdvanceBuffer(int len);
  void EnableChannel(int ch, bool enabled);
  bool ChannelEnabled(int channel);

 private:
  Uint8 delay_buffer_[2*SampleRate];
  int buffer_index_ = 0;
  int milliseconds_ = 250;
  float feedback_ = 0.2f;
  bool channel_enabled_[9];
};

class SoundData {
 public:
  SoundData();
  ~SoundData();

  void PlaySample(int n);
  void PlaySampleFromKeycode(SDL_Keycode key);
  bool LoadSamples(const char** files);
  void AdvanceDelayBuffer(int len);
  DelayEffect* GetDelayEffect() { return delay_effect_.get(); }

 private:
   Mix_Chunk* samples_[9];
   std::unique_ptr<DelayEffect> delay_effect_;
};

#endif  // SOUND_DATA_H
