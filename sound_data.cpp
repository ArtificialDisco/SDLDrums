#include "sound_data.h"

#include <stdio.h>

DelayEffect::DelayEffect() {
  for (int i = 0; i < 9; i++) {
    channel_enabled_[i] = false;
  }
  for (int i = 0; i < 2*SampleRate; i++) {
    delay_buffer_[i] = 0;
  }
}

DelayEffect::~DelayEffect() {
  printf("~DelayEffect\n");
}

void DelayEffect::AddToBuffer(Mix_Chunk* chunk) {
  int nsamples = SampleRate / 1000.0 * milliseconds_;
  Sint16* chunk16 = (Sint16*)chunk->abuf;
  Sint16* delay16 = (Sint16*)delay_buffer_;
  int idx = buffer_index_;
  for (int i = 0; i < chunk->alen / 2; i += 2) {
    delay16[(idx + nsamples + i) % SampleRate] = chunk16[i];
    delay16[(idx + nsamples + i + 1) % SampleRate] = chunk16[i + 1];
  }
}

void DelayEffect::ApplyDelay(Uint8* stream, int len) {
  Sint16* stream16 = (Sint16*)stream;
  Sint16* delay16 = (Sint16*)delay_buffer_;
  int idx = buffer_index_;
  int i;
  for (i = 0; i < len/2; i += 2) {
    delay16[(idx + i + 1) % SampleRate] *= feedback_;
    delay16[(idx + i) % SampleRate] *= feedback_;

    stream16[i] = stream16[i + 1] * 0.5 +
      delay16[(idx + i + 1) % SampleRate] * 0.5;
    stream16[i + 1] = stream16[i] * 0.5 +
      delay16[(idx + i) % SampleRate] * 0.5;
  }
  AdvanceBuffer(i);
}

void DelayEffect::AdvanceBuffer(int len) {
  buffer_index_ += len;
  buffer_index_ %= 2*SampleRate;
}

void DelayEffect::EnableChannel(int ch, bool enabled) {
  channel_enabled_[ch] = enabled;
}

bool DelayEffect::ChannelEnabled(int channel) {
  return channel_enabled_[channel];
}

SoundData::SoundData() {
  delay_effect_ = std::make_unique<DelayEffect>();
}

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
  if (delay_effect_->ChannelEnabled(n)) {
    delay_effect_->AddToBuffer(samples_[n]);
  }
  Mix_PlayChannel(n, samples_[n], 0);
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

void SoundData::AdvanceDelayBuffer(int len) {
  delay_effect_->AdvanceBuffer(len);
}
