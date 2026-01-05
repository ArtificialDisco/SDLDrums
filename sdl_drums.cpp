#include <SDL.h>

#ifdef __linux__ 
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#elif _WIN32
#include <SDL_mixer.h>
#include <SDL_image.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <memory>

#include "sdl_drums.h"
#include "sound_button.h"
#include "drum_loop.h"
#include "sound_data.h"
#include "trig_button.h"
#include "control_button.h"
#include "step_button.h"

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

// Keep track of all surfaces so we can free them correctly
// Total surfaces are currently 46. Update this when needed.
SDL_Surface* all_surfaces[50];
int total_surfaces = 0;

void free_surfaces() {
  for (int i = 0; i < total_surfaces; i++) {
    SDL_FreeSurface(all_surfaces[i]);
  }
}

SDL_Window* window;
SDL_Surface* screen = NULL;

Uint32 next_time;

Uint32 time_left(void)
{
	Uint32 now;
	now = SDL_GetTicks();
	if(next_time <= now)
		return 0;
	else
		return next_time - now;
}

bool init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not be initialized: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow("SDL Drums", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("Could not open window: %s\n", SDL_GetError());
    return false;
  }

  if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) != 0) {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
           Mix_GetError());
    return false;
  }

  if (Mix_AllocateChannels(32) < 0) {
    fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
    exit(-1);
  }

  screen = SDL_GetWindowSurface(window);
  SDL_SetSurfaceBlendMode(screen, SDL_BLENDMODE_BLEND);
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  SDL_UpdateWindowSurface(window);
  return true;
}

SDL_Surface *load_surface(SDL_Surface *screen, const char *path,
                          bool optimize = true) {
  SDL_Surface* optimizedSurface = NULL;

  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL) {
    printf( "Unable to load image %s!\nSDL_image Error: %s\n", path, IMG_GetError() );
  } else {
    if (!optimize) {
      all_surfaces[total_surfaces++] = optimizedSurface;
      return loadedSurface;
    }
    optimizedSurface = SDL_ConvertSurface(loadedSurface, screen->format, 0);
    if (optimizedSurface == NULL) {
        printf( "Unable to optimize image %s!\nSDL Error: %s\n", path, SDL_GetError() );
    }

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  all_surfaces[total_surfaces++] = optimizedSurface;
  return optimizedSurface;
}

bool load_sound_button_imgs(SDL_Surface* screen,
                            SDL_Surface* buttons_inactive[SOUND_BUTTONS_TOTAL],
                            SDL_Surface* buttons_active[SOUND_BUTTONS_TOTAL]) {
  // Make sure all surfaces are init'd as null, since if we fail at file
  // number i we need to free every surface from 0 to i, which involves
  // null-checking.
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    buttons_inactive[i] = buttons_active[i] = nullptr;
  }

  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    buttons_inactive[i] = load_surface(screen, sound_buttons_inactive_files[i]);
    buttons_active[i] = load_surface(screen, sound_buttons_active_files[i]);
    if (buttons_inactive[i] == NULL || buttons_active[i] == NULL) {
      return false;
    }
  }
  return true;
}

bool load_trig_button_imgs(
    SDL_Surface* screen, SDL_Surface *trig_button_icons[SOUND_BUTTONS_TOTAL]) {
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    trig_button_icons[i] = nullptr;
  }
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    trig_button_icons[i] = load_surface(screen, trig_buttons_files[i], false);
    if (trig_button_icons[i] == NULL) {
      return false;
    }
  }
  return true;
}

bool load_step_button_imgs(SDL_Surface *screen,
                           SDL_Surface *step_button_icons[STEP_BUTTONS_TOTAL]) {
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_button_icons[i] = nullptr;
  }
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_button_icons[i] = load_surface(screen, step_button_files[i], false);
    if (step_button_icons[i] == NULL) {
      return false;
    }
  }
  return true;
}

// TODO: Repetition from update_trigs. 
bool update_trigs_from_pattern(DrumLoop::Pattern* p,
    std::unique_ptr<TrigButton> trig_buttons[SOUND_BUTTONS_TOTAL]
                                            [STEPS_TOTAL]) {
  bool screen_needs_update = false;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 32; j++) {
      trig_buttons[i][j]->SetEnabled(p->tracks[i][j] == '1' ?
                                     true : false, false);
      screen_needs_update |=
          trig_buttons[i][j]->UpdateStep();
    }
  }
  return screen_needs_update;
}

bool clear_and_update_trigs(std::unique_ptr<TrigButton> trig_buttons
    [SOUND_BUTTONS_TOTAL][STEPS_TOTAL]) {
  bool screen_needs_update = false;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 32; j++) {
      trig_buttons[i][j]->SetEnabled(false, false);
      screen_needs_update |=
        trig_buttons[i][j]->UpdateStep();
    }
  }
  return screen_needs_update;
}

bool update_trigs(std::unique_ptr<TrigButton> trig_buttons[SOUND_BUTTONS_TOTAL]
                                                          [STEPS_TOTAL]) {
  bool screen_needs_update = false;
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    for (int j = 0; j < STEPS_TOTAL; j++) {
      screen_needs_update |=
          trig_buttons[i][j]->UpdateStep();
    }
  }
  return screen_needs_update;
}

// True for undo, false for redo. Maybe confusing? Should use enum despite the boolean
// nature of this?
void apply_undo_action(DrumLoop::UndoAction action,
    std::unique_ptr<TrigButton> trig_buttons[SOUND_BUTTONS_TOTAL][STEPS_TOTAL],
    bool undo) {
  if (action.type == DrumLoop::TrigEdit) {
    DrumLoop::TrigEntry* entry = ((DrumLoop::TrigEntry*)(action.data));
    int track = entry->track;
    int step = entry->step;
    int data = entry->data;
    // !undo of course means redo
    bool value = (undo && data == '0') ||
                 (!undo && data == '1') ? true : false;
    trig_buttons[track][step]->
      SetEnabled(value, false);
    update_trigs(trig_buttons);
  } else if (action.type == DrumLoop::ClearAll) {
    if (undo) {
      DrumLoop::Pattern* p = (DrumLoop::Pattern*)(action.data);
      update_trigs_from_pattern(p, trig_buttons);
    } else {
      clear_and_update_trigs(trig_buttons);
    }
  }
}

void close_program() {
  SDL_DestroyWindow(window);
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* argv[]) {
  SDL_Surface* sound_buttons_inactive[SOUND_BUTTONS_TOTAL];
  SDL_Surface* sound_buttons_active[SOUND_BUTTONS_TOTAL];
  SDL_Surface* trig_button_icons[SOUND_BUTTONS_TOTAL];
  SDL_Surface* step_button_icons[STEP_BUTTONS_TOTAL];

  SDL_Surface* play_button_inactive_surface;
  SDL_Surface* play_button_active_surface;
  SDL_Surface* stop_button_surface;
  SDL_Surface* rec_button_surface;
  SDL_Surface* pause_button_surface;
  SDL_Surface* pause_button_toggled_surface;

  SDL_Surface* undo_button_surface;
  SDL_Surface* redo_button_surface;
  SDL_Surface* clear_button_surface;

  std::unique_ptr<SoundButton> sound_buttons[SOUND_BUTTONS_TOTAL];
  std::unique_ptr<TrigButton> trig_buttons[SOUND_BUTTONS_TOTAL][STEPS_TOTAL];
  std::unique_ptr<StepButton> step_buttons[STEP_BUTTONS_TOTAL];

  std::unique_ptr<ControlButton> play_button;
  std::unique_ptr<ControlButton> rec_button;
  std::unique_ptr<ControlButton> pause_button;

  std::unique_ptr<Button> undo_button;
  std::unique_ptr<Button> redo_button;
  std::unique_ptr<Button> clear_button;

  SDL_Keycode keyshortcuts[SOUND_BUTTONS_TOTAL] = {
    SDLK_z, SDLK_x, SDLK_c,
    SDLK_a, SDLK_s, SDLK_d,
    SDLK_q, SDLK_w, SDLK_e
  };
  SoundData sound_data;

  if (!init_sdl()) {
    close_program();
    return INIT_FAILED;
  }

  if (!sound_data.LoadSamples(samples_files)) {
    close_program();
    return INIT_FAILED;
  }

  play_button_inactive_surface =
      load_surface(screen, play_button_inactive_file, false);
  play_button_active_surface =
      load_surface(screen, play_button_active_file, false);
  stop_button_surface =
      load_surface(screen, stop_button_file, false);
  rec_button_surface = load_surface(screen, rec_button_file, false);
  pause_button_surface = load_surface(screen, pause_button_file, false);
  pause_button_toggled_surface = load_surface(screen,
                                             pause_button_toggled_file, false);

  if (!play_button_inactive_surface || !play_button_active_surface ||
      !stop_button_surface || !rec_button_surface || !pause_button_surface ||
      !pause_button_toggled_surface) {
    free_surfaces();
    close_program();
    return INIT_FAILED;
  }

  if (!load_sound_button_imgs(screen, sound_buttons_inactive,
                              sound_buttons_active)) {
    free_surfaces();
    close_program();
    return INIT_FAILED;
  }

  if (!load_trig_button_imgs(screen, trig_button_icons)) {
    free_surfaces();
    close_program();
    return INIT_FAILED; 
  }

  if (!load_step_button_imgs(screen, step_button_icons)) {
    free_surfaces();
    close_program();
    return INIT_FAILED;
  }

  undo_button_surface =
      load_surface(screen, undo_button_inactive_file);
  redo_button_surface =
      load_surface(screen, redo_button_inactive_file);
  clear_button_surface =
      load_surface(screen, clear_button_inactive_file);

  if (!undo_button_surface || !redo_button_surface || !clear_button_surface) {
    free_surfaces();
    close_program();
    return INIT_FAILED;
  }

  // Init sound pads
  int button_pos_x = 30;
  int button_pos_y = 30 + 2*SOUND_BUTTON_HEIGHT;
  for (unsigned i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    SDL_Rect rect;
    rect.x = button_pos_x;
    rect.y = button_pos_y;
    rect.w = SOUND_BUTTON_WIDTH;
    rect.h = SOUND_BUTTON_HEIGHT;
    sound_buttons[i] = std::make_unique<SoundButton>(
        screen, sound_buttons_inactive[i], sound_buttons_active[i], rect,
        keyshortcuts[i], &sound_data);
    sound_buttons[i]->Draw();
    
    button_pos_x += (SOUND_BUTTON_WIDTH);
    if ((i+1) % 3 == 0) {
      button_pos_x = 30;
      button_pos_y -= (SOUND_BUTTON_HEIGHT);
    }
  }

  // Init drum loop and create sequencer
  DrumLoop drum_loop(&sound_data);

  SDL_Surface* empty_slot_surface = load_surface(screen, empty_slot);
  SDL_Surface* active_empty_slot_surface = load_surface(screen, active_empty_slot);

  SDL_Rect trig_rect;
  trig_rect.w = empty_slot_surface->w;
  trig_rect.h = empty_slot_surface->h;
  trig_rect.x = 30;
  trig_rect.y = SCREEN_HEIGHT - 30;
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    trig_rect.x = 30;
    trig_rect.y -= 27;
    for (int j = 0; j < STEPS_TOTAL; j++) {
      trig_buttons[i][j] = std::make_unique<TrigButton>(
          screen, active_empty_slot_surface, empty_slot_surface,
          trig_button_icons[i], trig_rect, &drum_loop);

      // TODO: A little counter-intuitive having to do this.
      trig_buttons[i][j]->SetInactive();

      if(drum_loop.GetTrig(i, j) != '0') {
        trig_buttons[i][j]->Enable();
      }
      trig_buttons[i][j]->SetTrack(i);
      trig_buttons[i][j]->SetStep(j);
      trig_buttons[i][j]->Draw();

      trig_rect.x += 27;
      if ((j+1) % 4 == 0) {
        if ((j+1) % 16 == 0) {
          trig_rect.x += 15;
        }
        trig_rect.x += 8;
      }
    }
  }

  // Step buttons
  SDL_Rect step_rect;
  step_rect.x = 35;
  step_rect.y = trig_rect.y - step_button_icons[0]->h - 5;
  step_rect.w = step_button_icons[0]->w;
  step_rect.h = step_button_icons[0]->h;
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_buttons[i] = std::make_unique<StepButton>(screen, step_button_icons[i],
                                                   step_rect, i+1);
    step_buttons[i]->Draw();

    step_rect.x += 27;
    if ((i+1) % 4 == 0) {
      if ((i+1) % 16 == 0) {
        step_rect.x += 15;
      }
      step_rect.x += 8;
    }
  }

  // Control buttons
  SDL_Rect play_rect;
  play_rect.x = 30;
  play_rect.y = SCREEN_HEIGHT - 400;
  play_rect.w = CONTROL_BUTTON_WIDTH;
  play_rect.h = CONTROL_BUTTON_HEIGHT;
  play_button = std::make_unique<ControlButton>(
      screen, play_button_active_surface, play_button_inactive_surface,
      stop_button_surface, play_rect, SDLK_p, SDLK_UNKNOWN);
  play_button->Draw();

  SDL_Rect pause_rect;
  pause_rect.x = play_rect.x + CONTROL_BUTTON_WIDTH;
  pause_rect.y = SCREEN_HEIGHT - 400;
  pause_rect.w = CONTROL_BUTTON_WIDTH;
  pause_rect.h = CONTROL_BUTTON_HEIGHT;
  pause_button = std::make_unique<ControlButton>(
      screen, nullptr, pause_button_surface,
      pause_button_toggled_surface, pause_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  pause_button->Draw();

  SDL_Rect rec_rect;
  rec_rect.x = pause_rect.x + CONTROL_BUTTON_WIDTH;
  rec_rect.y = SCREEN_HEIGHT - 400;
  rec_rect.w = CONTROL_BUTTON_WIDTH;
  rec_rect.h = CONTROL_BUTTON_HEIGHT;
  rec_button = std::make_unique<ControlButton>(
      screen, nullptr, rec_button_surface,
      stop_button_surface, rec_rect, SDLK_r, SDLK_UNKNOWN);
  rec_button->Draw();

  // Edit buttons
  SDL_Rect edit_rect;
  edit_rect.x = SCREEN_WIDTH - undo_button_surface->w - 70;
  edit_rect.y = 30;
  edit_rect.w = 200;
  edit_rect.h = 50;
  undo_button = std::make_unique<Button>(screen, nullptr,
      undo_button_surface, undo_button_surface, edit_rect,
      SDLK_u, SDLK_UNKNOWN);
  //undo_button->Draw();
  SDL_BlitSurface(undo_button_surface, NULL, screen, &edit_rect);
  
  edit_rect.y += 10 + redo_button_surface->h;
  redo_button = std::make_unique<Button>(screen, nullptr,
      redo_button_surface, redo_button_surface, edit_rect,
      SDLK_r, SDLK_UNKNOWN);
  //redo_button->Draw();
  SDL_BlitSurface(redo_button_surface, NULL, screen, &edit_rect);

  edit_rect.y += 10 + clear_button_surface->h;
  edit_rect.w = 200;
  edit_rect.h = 50;
  clear_button = std::make_unique<Button>(screen, nullptr,
      clear_button_surface, clear_button_surface, edit_rect,
      SDLK_l, SDLK_UNKNOWN);
  //undo_button->Draw();
  SDL_BlitSurface(clear_button_surface, NULL, screen, &edit_rect);

  SDL_UpdateWindowSurface(window);
  
  // Main event loop
  SDL_Event e;
  bool quit = false;
  next_time = SDL_GetTicks() + TICK_INTERVAL;
  int current_step = -1;
  bool screen_needs_update;
  while (quit == false) {
    screen_needs_update = false;
    if (drum_loop.Running()) { 
      int step = drum_loop.CurrentStep();
      if (step != current_step) {
        screen_needs_update = update_trigs(trig_buttons);
        current_step = step;
      }
    }

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }

      if (e.type == SDL_MOUSEMOTION) {
          next_time += TICK_INTERVAL;
          SDL_Delay(time_left());
          continue;
      }

      ////////////////////////////////
      //   REFACTOR CTRL BUTTONS?   //
      ////////////////////////////////
      bool play_clicked = false;
      screen_needs_update |=
          play_button->HandleEvent(&e, &play_clicked);
      if (play_clicked) {
        if (drum_loop.Running()) {
          if (drum_loop.Recording()) {
            play_button->SetToggled(true);
            rec_button->SetToggled(false);
            pause_button->SetToggled(false);
            drum_loop.SetRec(false);
          } else {
            drum_loop.Stop();
            update_trigs(trig_buttons);
            play_button->SetToggled(false);
            pause_button->SetToggled(false);
          }
        } else {
          if (drum_loop.Paused() && !drum_loop.RecMode()) {
            drum_loop.Stop();
            play_button->SetToggled(false);
            pause_button->SetToggled(false);
            update_trigs(trig_buttons);
          } else {
            drum_loop.Start();
            play_button->SetToggled(true);
            rec_button->SetToggled(false);
            pause_button->SetToggled(false);
          }
        }
      }

      bool rec_clicked = false;
      screen_needs_update |=
          rec_button->HandleEvent(&e, &rec_clicked);
      if (rec_clicked) {
        if (drum_loop.Running()) {
          if (!drum_loop.RecMode()) {
            play_button->SetToggled(false);
            rec_button->SetToggled(true);
            drum_loop.SetRec(true);
          } else {
            rec_button->SetToggled(false);
            drum_loop.Stop();
            update_trigs(trig_buttons);
          }
        } else {
          if (drum_loop.Paused() && drum_loop.RecMode()) {
            play_button->SetToggled(false);
            rec_button->SetToggled(false);
            pause_button->SetToggled(false);
            drum_loop.Stop();
            update_trigs(trig_buttons);
          } else {
            play_button->SetToggled(false);
            drum_loop.StartWithRec();
            rec_button->SetToggled(true);
            pause_button->SetToggled(false);
          }
        }
      }

      bool pause_clicked = false;
      screen_needs_update |=
          pause_button->HandleEvent(&e, &pause_clicked);
      if (pause_clicked) {
        if (drum_loop.Running()) {
          drum_loop.Pause();
          update_trigs(trig_buttons);
          pause_button->SetToggled(true);
        } else {
          if (drum_loop.Paused()) {
            if (drum_loop.RecMode()) {
              drum_loop.StartWithRec();
              pause_button->SetToggled(false);
            } else {
              drum_loop.Start();
              pause_button->SetToggled(false);
            }
            //play_button->SetToggled(false);
          }
        }
      }
      ////////////////////////////////

      bool mousedown = false;
      bool clear_button_clicked = false;
      clear_button->HandleEventBase(&e, &mousedown, &clear_button_clicked);
      if (clear_button_clicked) {
        
        // Currently we just do this so that Clear will be added to the undo
        // list. The SetEnabled's on the trigs will also zero ou the drum loop.
        // Revisit this.
        drum_loop.ClearPattern();

        for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
          for (int j = 0; j < STEPS_TOTAL; j++)
            trig_buttons[i][j]->SetEnabled(false, false);
        }
        update_trigs(trig_buttons);
        screen_needs_update = true;
      }

      bool undo_button_clicked = false;
      undo_button->HandleEventBase(&e, &mousedown,
          &undo_button_clicked);
      if (undo_button_clicked) {
        DrumLoop::UndoAction action = drum_loop.Undo();
        apply_undo_action(action, trig_buttons, true);
        screen_needs_update = true;
      }

      bool redo_button_clicked = false;
      redo_button->HandleEventBase(&e, &mousedown, &redo_button_clicked);
      if (redo_button_clicked) {
        DrumLoop::UndoAction action = drum_loop.Redo();
        apply_undo_action(action, trig_buttons, false);
        screen_needs_update = true;
      }

      for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
        bool clicked = false;
        screen_needs_update |=
            sound_buttons[i]->HandleEvent(&e, &clicked);
        if (clicked && (drum_loop.Recording() || drum_loop.Paused())) {
          int step = drum_loop.CurrentStep();
          trig_buttons[i][step]->HandleClick();
        }
      }
      for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
        for (int j = 0; j < STEPS_TOTAL; j++)
        screen_needs_update |=
            trig_buttons[i][j]->HandleEvent(&e);
      }
      bool step_button_clicked = false;
      for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
        screen_needs_update |= step_buttons[i]->
            HandleEvent(&e, &step_button_clicked);
        if (step_button_clicked) {
          break;
        }
      }
      
      // Key event handling that is not dealt with inside the button classes
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_SPACE:
            if (drum_loop.Running()) {
              drum_loop.Stop();
              screen_needs_update |= update_trigs(trig_buttons);
              play_button->SetToggled(false);
              rec_button->SetToggled(false);
            } else {
              drum_loop.Start();
              play_button->SetToggled(true);
            }
            break;
          case SDLK_ESCAPE:
            quit = true;
            break;
          case SDLK_UP:
            drum_loop.SpeedUp(10);
            break;
          case SDLK_DOWN:
            drum_loop.SlowDown(10);
            break;
          case SDLK_RIGHT:
            drum_loop.NextStep();
            screen_needs_update = update_trigs(trig_buttons);
            break;
          case SDLK_LEFT:
            drum_loop.PrevStep();
            screen_needs_update = update_trigs(trig_buttons);
            break;
          case SDLK_b:
            printf("%i\n", drum_loop.CurrentStep());
            break;
        }
      }
    }
    if (screen_needs_update) {
      SDL_UpdateWindowSurface(window);
      screen_needs_update = false;
    }
    next_time += TICK_INTERVAL;
    SDL_Delay(time_left());
  }

  // Cleanup everything
  free_surfaces();
  close_program();
  return 0;
}

