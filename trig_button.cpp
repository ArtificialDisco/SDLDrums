#include "trig_button.h"
#include <stdio.h>

TrigButton::TrigButton(SDL_Surface *screen, SDL_Surface *active_empty_slot,
                       SDL_Surface *empty_slot, SDL_Surface *toggled,
                       SDL_Rect rect, DrumLoop *drum_loop)
    : Button(screen, active_empty_slot, empty_slot, toggled, rect, SDLK_UNKNOWN,
             SDLK_UNKNOWN) {
  drum_loop_ = drum_loop;
}

void TrigButton::Draw() {
  if (toggled_) {
    SetToggled(true);
  } else {
    SetInactive();
  }
}

bool TrigButton::UpdateStep() {
  bool stepped = false;
  int current_step = drum_loop_->CurrentStep();

  /*if (!drum_loop_->Running()) {
    return false;
  }*/

  if (step_ == current_step) {
    if (!active_step_) {
      SetActive();
      active_step_ = true;
      stepped = true;
      if (toggled_) {
        SetToggled(true);
      }
    }
  } else {
    if (active_step_) {
      SetInactive();
      active_step_ = false;
      stepped = true;
      if (toggled_) {
        SetToggled(true);
      }
    }
  }
  return stepped;
}

bool TrigButton::HandleClick() {
  if (toggled_) {
    toggled_ = false;
    Draw();
    drum_loop_->SetTrig(track_, step_, '0');
    return true;
  } else {
    toggled_ = true;
    Draw();
    drum_loop_->SetTrig(track_, step_, '1');
    return true;
  }
}

void TrigButton::SetEnabled(bool enabled, bool undoable) {
  SetToggled(enabled);
  toggled_ = enabled;
  active_step_ = enabled;
  if (enabled) {
    SetActive();
    drum_loop_->SetTrig(track_, step_, '1', undoable);
  } else {
    SetInactive();
    drum_loop_->SetTrig(track_, step_, '0', undoable);
  }
}

bool TrigButton::HandleEvent(SDL_Event* e) {
  bool clicked = false;
  bool mousedown = false;
  Button::HandleEventBase(e, &mousedown, &clicked);

  if (clicked) {
    HandleClick();
  }
  return clicked;
}
