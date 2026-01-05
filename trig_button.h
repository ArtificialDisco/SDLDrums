#ifndef TRIG_BUTTON_H
#define TRIG_BUTTON_H

#include <SDL.h>

#include "button.h"
#include "drum_loop.h"

class TrigButton : public Button
{
 public:
   TrigButton(SDL_Surface *screen, SDL_Surface *active_empty_slot,
              SDL_Surface *empty_slot, SDL_Surface *toggled, SDL_Rect rect,
              DrumLoop *drum_loop);

   bool HandleEvent(SDL_Event *e);
   void Draw();

   bool UpdateStep();
   bool HandleClick();
   void SetEnabled(bool enabled, bool undoable);

   void SetTrack(int track) { track_ = track; }
   void SetStep(int step) { step_ = step; }
   void Enable() { toggled_ = true; }

 private:
   DrumLoop *drum_loop_;
   bool toggled_ = false;
   bool active_step_ = false;
   int track_;
   int step_;
};

#endif  // TRIG_BUTTON_H
