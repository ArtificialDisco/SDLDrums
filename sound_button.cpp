#include <SDL.h>

#include "sound_button.h"
#include "sound_data.h"

SoundButton::SoundButton(SDL_Surface *screen, SDL_Surface *inactive,
                         SDL_Surface *active, SDL_Rect rect,
                         SDL_Keycode keyshortcut, SoundData *sound_data)
    : Button(screen, active, inactive, nullptr, rect, keyshortcut, SDLK_UNKNOWN) {
  sound_data_ = sound_data;
}

void SoundButton::PlaySample() {
  sound_data_->PlaySampleFromKeycode(GetKeyShortcut());
}

void SoundButton::Draw() {
  if (Active()) {
    SetActive();
  } else {
    SetInactive();
  }
}

bool SoundButton::HandleEvent(SDL_Event* e, bool* clicked) {
  //bool clicked = false;
  bool screen_change = Button::HandleEventBase(e, nullptr, clicked);
  if (screen_change) {
    Draw();
  }
  if (*clicked) {
    PlaySample();
  }
  return screen_change;
}
