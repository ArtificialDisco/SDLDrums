#ifndef STEP_BUTTON_H
#define STEP_BUTTON_H

#include <SDL.h>

#include "button.h"

class StepButton : public Button
{
 public:
  StepButton(SDL_Surface* screen, SDL_Surface* inactive, SDL_Rect rect,
             int nstep);

  bool HandleEvent(SDL_Event* e, bool* clicked);
  void Draw();

 private:
  SDL_KeyCode GetKey(int nstep);

  SDL_Surface* screen_;
  int nstep_;
};

#endif  // STEP_BUTTON_H
