#ifndef CONTROL_BUTTON_H
#define CONTROL_BUTTON_H

#include "button.h"

class ControlButton : public Button
{
 public:
  ControlButton(SDL_Surface* screen,
                SDL_Surface* inactive,
                SDL_Surface* active,
                SDL_Surface* alt,
                SDL_Rect rect,
                SDL_Keycode keyshortcut1,
                SDL_Keycode keyshortcut2);

  bool HandleEvent(SDL_Event* e, bool* clicked);
  void Draw();
};

#endif  // CONTROL_BUTTON_H
