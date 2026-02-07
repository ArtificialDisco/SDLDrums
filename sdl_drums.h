#ifndef SDL_DRUMS_H
#define SDL_DRUMS_H

#include <SDL.h>
#include <memory>

#include "sound_button.h"
#include "control_button.h"
#include "trig_button.h"
#include "step_button.h"

// State of drum machine


const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const int SOUND_BUTTONS_TOTAL = 9;
const int STEP_BUTTONS_TOTAL = 8;
const int STEPS_TOTAL = 32;
const int TICK_INTERVAL = 10;

const int SOUND_BUTTON_WIDTH = 82;
const int SOUND_BUTTON_HEIGHT = 82;

const int CONTROL_BUTTON_WIDTH = 82;
const int CONTROL_BUTTON_HEIGHT = 82;

const int X_MARGIN = 40;
const int Y_MARGIN = 30;

class SDLDrums {
 public:
  SDLDrums();
  ~SDLDrums();
  int Run();

  // Error codes
  const int INIT_FAILED = 1;

  bool InitSDL();
  int InitAllSurfaces(SDL_Surface* screen);
  void CloseProgram();

  bool LoadSoundButtonImgs();
  bool LoadTrigButtonImgs(SDL_Surface* screen);
  bool LoadStepButtonImgs(SDL_Surface* screen);
  bool LoadDigits(SDL_Surface* screen);
  bool UpdateTrigsFromPattern(DrumLoop::Pattern* p);
  bool ClearAndUpdateTrigs();
  bool UpdateTrigs();

  void DrawBPM(SDL_Surface* surface, SDL_Rect rect, int bpm);
  void InitBPMButtons();
  bool HandleBPM(SDL_Event* e);

  void ApplyUndoAction(DrumLoop::UndoAction action, bool undo);
  void MixFunc(void* udata, Uint8* stream, int len);
  void InitDrumTriggersArea();
  void DrawDelayFXArea();
  bool HandleDelay(SDL_Event* e);
  void DrawDelayTimeValue();
  void DrawDelayFeedbackValue();

 private:
  SDL_Keycode sound_button_keys[SOUND_BUTTONS_TOTAL] = {
    SDLK_z, SDLK_x, SDLK_c,
    SDLK_a, SDLK_s, SDLK_d,
    SDLK_q, SDLK_w, SDLK_e
  };
  SoundData sound_data;
  std::unique_ptr<DrumLoop> drum_loop;
  SDL_Rect bpm_indicator_rect_;
  SDL_Rect delay_area_rect_;

  std::unique_ptr<SoundButton> sound_buttons[SOUND_BUTTONS_TOTAL];
  std::unique_ptr<TrigButton> trig_buttons[SOUND_BUTTONS_TOTAL][STEPS_TOTAL];
  std::unique_ptr<StepButton> step_buttons[STEP_BUTTONS_TOTAL];

  std::unique_ptr<ControlButton> play_button;
  std::unique_ptr<ControlButton> rec_button;
  std::unique_ptr<ControlButton> pause_button;

  std::unique_ptr<Button> undo_button;
  std::unique_ptr<Button> redo_button;
  std::unique_ptr<Button> clear_button;

  std::unique_ptr<Button> bpm_10_up_button;
  std::unique_ptr<Button> bpm_10_down_button;
  std::unique_ptr<Button> bpm_1_up_button;
  std::unique_ptr<Button> bpm_1_down_button;

  std::unique_ptr<Button> delay_feedback_incr_button;
  std::unique_ptr<Button> delay_feedback_decr_button;
  std::unique_ptr<Button> delay_time_incr_button;
  std::unique_ptr<Button> delay_time_decr_button;

  std::unique_ptr<Button> fx_button[9];

  SDL_Surface* sound_buttons_inactive[SOUND_BUTTONS_TOTAL];
  SDL_Surface* sound_buttons_active[SOUND_BUTTONS_TOTAL];
  SDL_Surface* trig_button_icons[SOUND_BUTTONS_TOTAL];
  SDL_Surface* step_button_icons[STEP_BUTTONS_TOTAL];
  SDL_Surface* digit_imgs[10];

  SDL_Surface* play_button_inactive_surface;
  SDL_Surface* play_button_active_surface;
  SDL_Surface* stop_button_surface;
  SDL_Surface* rec_button_surface;
  SDL_Surface* pause_button_surface;
  SDL_Surface* pause_button_toggled_surface;

  SDL_Surface* undo_button_surface;
  SDL_Surface* redo_button_surface;
  SDL_Surface* clear_button_surface;

  SDL_Surface* bpm_up_10_inactive_surface;
  SDL_Surface* bpm_up_10_active_surface;
  SDL_Surface* bpm_up_1_inactive_surface;
  SDL_Surface* bpm_up_1_active_surface;

  SDL_Surface* bpm_down_10_inactive_surface;
  SDL_Surface* bpm_down_10_active_surface;
  SDL_Surface* bpm_down_1_inactive_surface;
  SDL_Surface* bpm_down_1_active_surface;
  SDL_Surface* bpm_empty_surface;

  SDL_Surface* fx1_on;
  SDL_Surface* fx1_off;

  SDL_Surface* fx1_delay_area_surface;
  SDL_Surface* fx1_delay_right_inactive_surface;
  SDL_Surface* fx1_delay_right_active_surface;
  SDL_Surface* fx1_delay_left_inactive_surface;
  SDL_Surface* fx1_delay_left_active_surface;
  SDL_Surface* fx1_delay_digits_surface;

  SDL_Surface* empty_slot_surface;
  SDL_Surface* active_empty_slot_surface;
};

const char* samples_files[] = {
  "./samples/BD_Viscount_01.wav",
  "./samples/SD_Viscount_01.wav",
  "./samples/Cymbal_Hard_Open_Viscount_04.wav",
  "./samples/Cymbal_Hard_Open_Viscount_06.wav",
  "./samples/Clave_Viscount_01.wav",
  "./samples/Cymbal_Lite_Viscount_05_RR2.wav",
  "./samples/Tom_Hi_Viscount.wav",
  "./samples/Tom_Lo_Viscount.wav",
  "./samples/Handclap_Viscount_04.wav"
};

const char* sound_buttons_active_files[] = {
  "./images/sound_buttons/button01_active_c.png",
  "./images/sound_buttons/button02_active_c.png",
  "./images/sound_buttons/button03_active_c.png",
  "./images/sound_buttons/button04_active_c.png",
  "./images/sound_buttons/button05_active_c.png",
  "./images/sound_buttons/button06_active_c.png",
  "./images/sound_buttons/button07_active_c.png",
  "./images/sound_buttons/button08_active_c.png",
  "./images/sound_buttons/button09_active_c.png",
};

const char* sound_buttons_inactive_files[] = {
  "./images/sound_buttons/button01_inactive_c.png",
  "./images/sound_buttons/button02_inactive_c.png",
  "./images/sound_buttons/button03_inactive_c.png",
  "./images/sound_buttons/button04_inactive_c.png",
  "./images/sound_buttons/button05_inactive_c.png",
  "./images/sound_buttons/button06_inactive_c.png",
  "./images/sound_buttons/button07_inactive_c.png",
  "./images/sound_buttons/button08_inactive_c.png",
  "./images/sound_buttons/button09_inactive_c.png",
};

const char* trig_buttons_files[] = {
  "./images/icons_25/icon_1b.png",
  "./images/icons_25/icon_2b.png",
  "./images/icons_25/icon_3b.png",
  "./images/icons_25/icon_4b.png",
  "./images/icons_25/icon_5b.png",
  "./images/icons_25/icon_6b.png",
  "./images/icons_25/icon_7b.png",
  "./images/icons_25/icon_8b.png",
  "./images/icons_25/icon_9b.png",
};

const char* step_button_files[] = {
  "./images/steps/one.png",
  "./images/steps/two.png",
  "./images/steps/three.png",
  "./images/steps/four.png",
  "./images/steps/five.png",
  "./images/steps/six.png",
  "./images/steps/seven.png",
  "./images/steps/eight.png",
};

const char* digit_files[] = {
  "./images/digits/0.png",
  "./images/digits/1.png",
  "./images/digits/2.png",
  "./images/digits/3.png",
  "./images/digits/4.png",
  "./images/digits/5.png",
  "./images/digits/6.png",
  "./images/digits/7.png",
  "./images/digits/8.png",
  "./images/digits/9.png",
};

const char* fx1_on_file = "./images/icons_25/fx1_on.png";
const char* fx1_off_file = "./images/icons_25/fx1_off.png";

const char* fx1_delay_area_file = "./images/delay/fx1_delay_area_sans_serif_bold.png";
const char* fx1_delay_right_inactive_file = "./images/delay/delay_right_inactive.png";
const char* fx1_delay_right_active_file = "./images/delay/delay_right_active.png";
const char* fx1_delay_left_inactive_file = "./images/delay/delay_left_inactive.png";
const char* fx1_delay_left_active_file = "./images/delay/delay_left_active.png";
const char* fx1_delay_digits_file = "./images/delay/delay_digits.png";

const char* empty_slot = "./images/icons_25/empty_slot_b.png";
const char* active_empty_slot = "./images/icons_25/active_empty_slot.png";

const char* play_button_inactive_file = "./images/controls/play_inactive.png";
const char* play_button_active_file = "./images/controls/play_active.png";
const char* stop_button_file = "./images/controls/stop_inactive.png";
const char* rec_button_file = "./images/controls/rec_inactive.png";
const char* pause_button_file = "./images/controls/pause_inactive.png";
const char* pause_button_toggled_file = "./images/controls/pause_toggled.png";

const char* undo_button_inactive_file = "./images/edit_buttons/undo_button2_inactive.png";
const char* redo_button_inactive_file = "./images/edit_buttons/redo_button2_inactive.png";
const char* clear_button_inactive_file = "./images/edit_buttons/clear_button2_inactive.png";

const char* bpm_up_10_inactive_file = "./images/bpm/bpm_up_10_inactive.png";
const char* bpm_up_10_active_file = "./images/bpm/bpm_up_10_active.png";
const char* bpm_up_1_inactive_file = "./images/bpm/bpm_up_1_inactive.png";
const char* bpm_up_1_active_file = "./images/bpm/bpm_up_1_active.png";
const char* bpm_down_10_inactive_file = "./images/bpm/bpm_down_10_inactive.png";
const char* bpm_down_10_active_file = "./images/bpm/bpm_down_10_active.png";
const char* bpm_down_1_inactive_file = "./images/bpm/bpm_down_1_inactive.png";
const char* bpm_down_1_active_file = "./images/bpm/bpm_down_1_active.png";
const char* bpm_empty_file = "./images/bpm/bpm_empty.png";

#endif  // SDL_DRUMS_H
