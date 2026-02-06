#include "sound_data.h"

#include <stdio.h>

DelayEffect::DelayEffect() {
  for (int i = 0; i < 9; i++) {
    channel_enabled_[i] = false;
  }
  for (int i = 0; i < MaxBufferLength; i++) {
    delay_buffer_[i] = 0;
  }
  DelayLength = 2 * SampleRate / 1000 * milliseconds_;
}

DelayEffect::~DelayEffect() {
  printf("~DelayEffect\n");
}

void DelayEffect::AddToBuffer(Mix_Chunk* chunk) {
  int nsamples = SampleRate / 1000.0 * milliseconds_;
  Sint16* chunk16 = (Sint16*)chunk->abuf;
  Sint16* delay16 = (Sint16*)delay_buffer_;
  int idx = buffer_index_;

  if (chunk->alen < 2) {
    return;
  }

  Uint16 format;
  int channels;
  Mix_QuerySpec(NULL, &format, &channels);

  Uint8* tmp_delay = (Uint8*)malloc(sizeof(Uint8)*chunk->alen);
  for (int i = 0; i < chunk->alen; i++) {
    tmp_delay[i] = *(delay_buffer_ + ((buffer_index_ + nsamples * 4 + i) % (DelayLength)));
  }

  SDL_MixAudioFormat(tmp_delay, chunk->abuf, format, chunk->alen, SDL_MIX_MAXVOLUME);

  for (int i = 0; i < chunk->alen; i++) {
    *(delay_buffer_ + ((buffer_index_ + nsamples * 4 + i) % (DelayLength))) = tmp_delay[i];
  }

  free(tmp_delay);
}

void DelayEffect::ApplyDelay(Uint8* stream, int len) {
  Sint16* stream16 = (Sint16*)stream;
  Sint16* delay16 = (Sint16*)delay_buffer_;
  int idx = buffer_index_;

  if (len < 2) {
    return;
  }

  Uint16 format;
  int channels;
  Mix_QuerySpec(NULL, &format, &channels);
  Uint8* delay_tmp = (Uint8*)malloc(sizeof(Uint8)*len);

  for (int i = 0; i < len-1; i+=2) {
    delay_tmp[i] = *(delay_buffer_ + ((buffer_index_ + i) % DelayLength));
    delay_tmp[i+1] = *(delay_buffer_ + ((buffer_index_ + i+1) % DelayLength));
    ((Sint16*)delay_tmp)[i/2] *= feedback_;
    ((Sint16*)delay_tmp)[i/2+1] *= feedback_;
  }

  SDL_MixAudioFormat(stream, delay_tmp, format, len, SDL_MIX_MAXVOLUME);

  for (int i = 0; i < len; i++) {
    *(delay_buffer_ + ((buffer_index_ + i) % DelayLength)) = delay_tmp[i];
  }

  AdvanceBuffer(len);
  free(delay_tmp);
}

void DelayEffect::AdvanceBuffer(int len) {
  buffer_index_ += len;
  buffer_index_ %= DelayLength;
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
