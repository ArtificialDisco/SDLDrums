#include <SDL.h>

#include <stdio.h>

#include "button.h"

// A button becomes a toggle button (keeps a 'toggled' state') if the string
// |toggled| is not empty. A more ambitious button implementation would probably
// have a separate ToggleButton class.
// A button gets two key shortcuts, most will only have one. If we start needing
// more shortcuts for each buttons we'll revisit this -  use an array or
// something.
Button::Button(SDL_Surface *screen, SDL_Surface *active, SDL_Surface *inactive,
               SDL_Surface *toggled, SDL_Rect rect, SDL_Keycode keyshortcut1,
               SDL_Keycode keyshortcut2) {
  error_message_[0] = '\0';
  inactive_ = inactive;
  active_ = active;
  toggled_ = toggled;
  rect_ = rect;
  keyshortcut1_ = keyshortcut1;
  keyshortcut2_ = keyshortcut2;
  screen_ = screen;
}

Button::~Button() {}

void Button::SetPosition(int x, int y) {
  rect_.x = x;
  rect_.y = y;
}

void Button::SetActive() {
  SDL_BlitSurface(active_, NULL, screen_, &rect_);
}

void Button::SetInactive() {
  SDL_BlitSurface(inactive_, NULL, screen_, &rect_);
}

void Button::SetToggled(bool toggled) {

  if (toggled) {
    SDL_BlitSurface(toggled_, NULL, screen_, &rect_);
  } else {
    SDL_BlitSurface(inactive_, NULL, screen_, &rect_);
  }
  is_toggled_ = toggled;
}

bool Button::HandleEventBase(SDL_Event *e, bool *mousedown, bool *clicked) {
  bool needs_screen_update = false;

  if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
    if (e->key.keysym.sym == keyshortcut1_ ||
        e->key.keysym.sym == keyshortcut2_) {
      switch(e->type) {
        case SDL_KEYDOWN:
          *clicked = true;
          is_active_ = true;
          needs_screen_update = true;
          break;
        case SDL_KEYUP:
          is_active_ = false;
          needs_screen_update = true;
          break;
      }
    }
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    bool inside = true;
    if (x < rect_.x) {
      inside = false;
    } else if (x > rect_.x + rect_.w) {
      inside = false;
    } else if (y < rect_.y) {
      inside = false;
    } else if (y > rect_.y + rect_.h){
      inside = false;
    }

    switch (e->type) {
      case SDL_MOUSEBUTTONUP:
        if (is_active_) {
          is_active_ = false;
          needs_screen_update = true;
          if (inside && clicked != nullptr) {
            *clicked = true;
          }
        }
        is_active_ = false;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (inside) {
          is_active_ = true;
          if (mousedown != nullptr) {
            *mousedown = true;
          }
          needs_screen_update = true;
        }
        break;
    }
  }
  return needs_screen_update;
}
