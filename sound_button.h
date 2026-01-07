#ifndef SOUND_BUTTON_H
#define SOUND_BUTTON_H

#include "button.h"
#include "sound_data.h"

class SoundButton : public Button
{
 public:
   SoundButton(SDL_Surface *screen, SDL_Surface *inactive, SDL_Surface *active,
               SDL_Rect rect, SDL_Keycode keyshortcut, SoundData *sound_data);

   void PlaySample();
   bool HandleEvent(SDL_Event* e, bool* clicked) override;
   void Draw() override;

 private:
  SoundData* sound_data_;
};

#endif  // SOUND_BUTTON_H
