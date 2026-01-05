#include <stdio.h>
#include "step_button.h"

StepButton::StepButton(SDL_Surface* screen, SDL_Surface* inactive, SDL_Rect rect, int nstep)
  : Button(screen, nullptr, inactive, nullptr, rect, GetKey(nstep), SDLK_UNKNOWN) {
  screen_ = screen;
  nstep_ = nstep;
}

SDL_KeyCode StepButton::GetKey(int nstep) {
  switch (nstep) {
    case 1: return SDLK_1;
    case 2: return SDLK_2;
    case 3: return SDLK_3;
    case 4: return SDLK_4;
    case 5: return SDLK_5;
    case 6: return SDLK_6;
    case 7: return SDLK_7;
    case 8: return SDLK_8;
    default: return SDLK_UNKNOWN;
  }
}

void StepButton::Draw() {
  SetInactive();
}

bool StepButton::HandleEvent(SDL_Event* e, bool* clicked) {
  //bool clicked = false;
  bool screen_change = Button::HandleEventBase(e, nullptr, clicked);
  if (screen_change) {
    Draw();
  }
  return false;
}
