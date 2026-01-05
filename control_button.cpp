#include <SDL.h>

#include "control_button.h"

ControlButton::ControlButton(SDL_Surface *screen, SDL_Surface *active,
                             SDL_Surface *inactive, SDL_Surface *alt_inactive,
                             SDL_Rect rect, SDL_Keycode keyshortcut1,
                             SDL_Keycode keyshortcut2)
    : Button(screen, active, inactive, alt_inactive, rect, keyshortcut1,
             keyshortcut2) {}

void ControlButton::Draw() {
  if (Toggled())
    return;

  if (Active()) {
    SetActive();
  } else {
    SetInactive();
  }
}

bool ControlButton::HandleEvent(SDL_Event *e, bool *clicked) {
  *clicked = false;
  bool screen_change = Button::HandleEventBase(e, nullptr, clicked);
  if (screen_change) {
    Draw();
  }
  return screen_change;
}
