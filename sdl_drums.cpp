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
#include "drum_loop.h"
#include "sound_data.h"
#include "util.h"

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

/////////// TODO: Should belong to SDLDrums as well
SDL_Window* window;
SDL_Surface* screen = NULL;

SDL_Rect scope_rect = { 367, 175, 300, 200 };
void MixFunc(void* udata, Uint8* stream, int len) {
  SDL_Surface* surface = (SDL_Surface*)udata;

  SDL_Rect srcrect = { 0, 0, 300, 200 };
  //SDL_Rect dstrect = { 350, 125, 300, 200 };
  SDL_FillRect(surface, &srcrect, SDL_MapRGB(screen->format, 0, 0, 0));
  draw_sample(surface, stream, len, SDL_MapRGB(screen->format, 0xff, 0xb8, 0x2a));
  SDL_BlitSurface(surface, nullptr, screen, &scope_rect);
  SDL_UpdateWindowSurface(window);
}
//////////


bool SDLDrums::InitSDL() {
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

  if (Mix_OpenAudio(SampleRate, AUDIO_S16SYS, 2, 512) != 0) {
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



bool SDLDrums::LoadSoundButtonImgs() {
  // Make sure all surfaces are init'd as null, since if we fail at file
  // number i we need to free every surface from 0 to i, which involves
  // null-checking.
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    sound_buttons_inactive[i] = sound_buttons_active[i] = nullptr;
  }

  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    sound_buttons_inactive[i] = load_surface(screen, sound_buttons_inactive_files[i]);
    sound_buttons_active[i] = load_surface(screen, sound_buttons_active_files[i]);
    if (sound_buttons_inactive[i] == NULL || sound_buttons_active[i] == NULL) {
      return false;
    }
  }
  return true;
}

bool SDLDrums::LoadTrigButtonImgs(SDL_Surface* screen) {
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

bool SDLDrums::LoadStepButtonImgs(SDL_Surface *screen) {
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_button_icons[i] = nullptr;
  }
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_button_icons[i] = load_surface(screen, step_button_files[i], false);
    if (step_button_icons[i] == nullptr) {
      return false;
    }
  }
  return true;
}

bool SDLDrums::LoadDigits(SDL_Surface* screen) {
  
  for (int i = 0; i < 10; i++) {
    digit_imgs[i] = nullptr;
  }
  for (int i = 0; i < 10; i++) {
    digit_imgs[i] = load_surface(screen, digit_files[i]);
    if (digit_imgs[i] == nullptr) {
      return false;
    }
  }
  return true;
}

// TODO: Repetition from update_trigs. 
bool SDLDrums::UpdateTrigsFromPattern(DrumLoop::Pattern* p) {
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

bool SDLDrums::ClearAndUpdateTrigs() {
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

bool SDLDrums::UpdateTrigs() {
  bool screen_needs_update = false;
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    for (int j = 0; j < STEPS_TOTAL; j++) {
      screen_needs_update |=
          trig_buttons[i][j]->UpdateStep();
    }
  }
  return screen_needs_update;
}

void SDLDrums::DrawBPM(SDL_Surface* surface, SDL_Rect rect, int bpm) {
  short d3 = bpm % 10; bpm /= 10;
  short d2 = bpm % 10; bpm /= 10;
  short d1 = bpm % 10;
  int x_step = digit_imgs[d1]->w;
  
  SDL_BlitSurface(digit_imgs[d1], nullptr, surface, &rect);
  rect.x += x_step;
  SDL_BlitSurface(digit_imgs[d2], nullptr, surface, &rect);
  rect.x += x_step;
  SDL_BlitSurface(digit_imgs[d3], nullptr, surface, &rect);
  rect.x += x_step;
}

// True for undo, false for redo. Maybe confusing? Should use enum despite the boolean
// nature of this?
void SDLDrums::ApplyUndoAction(DrumLoop::UndoAction action, bool undo) {
  if (action.type == DrumLoop::TrigEdit) {
    DrumLoop::TrigEntry* entry = ((DrumLoop::TrigEntry*)(action.data));
    int track = entry->track;
    int step = entry->step;
    int data = entry->data;
    // !undo of course means redo
    bool value = (undo && data == '0') ||
                 (!undo && data == '1') ? true : false;
    trig_buttons[track][step]->SetEnabled(value, false);
    UpdateTrigs();
  } else if (action.type == DrumLoop::ClearAll) {
    if (undo) {
      DrumLoop::Pattern* p = (DrumLoop::Pattern*)(action.data);
      UpdateTrigsFromPattern(p);
    } else {
      ClearAndUpdateTrigs();
    }
  }
}

void SDLDrums::CloseProgram() {
  printf("Closing program.\n");
  SDL_DestroyWindow(window);
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
}

int SDLDrums::InitAllSurfaces(SDL_Surface* screen) {
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
    return INIT_FAILED;
  }

  bpm_up_10_inactive_surface = load_surface(screen, bpm_up_10_inactive_file);
  bpm_up_10_active_surface = load_surface(screen, bpm_up_10_active_file);
  bpm_up_1_inactive_surface = load_surface(screen, bpm_up_1_inactive_file);
  bpm_up_1_active_surface = load_surface(screen, bpm_up_1_active_file);
  bpm_down_10_inactive_surface = load_surface(screen, bpm_down_10_inactive_file);
  bpm_down_10_active_surface = load_surface(screen, bpm_down_10_active_file);
  bpm_down_1_inactive_surface = load_surface(screen, bpm_down_1_inactive_file);
  bpm_down_1_active_surface = load_surface(screen, bpm_down_1_active_file);
  bpm_empty_surface = load_surface(screen, bpm_empty_file);

  if (!bpm_up_10_inactive_surface || !bpm_up_10_active_surface ||
    !bpm_up_1_inactive_surface || !bpm_up_1_active_surface ||
    !bpm_down_10_inactive_surface || !bpm_down_10_active_surface ||
    !bpm_down_1_inactive_surface || !bpm_down_1_active_surface ||
    !bpm_empty_file) {
    return INIT_FAILED;
  }

  empty_slot_surface = load_surface(screen, empty_slot, false);
  active_empty_slot_surface = load_surface(screen, active_empty_slot, false);

  if (!empty_slot_surface || !active_empty_slot_surface) {
    return INIT_FAILED;
  }

  if (!LoadSoundButtonImgs()) {
    return INIT_FAILED;
  }

  if (!LoadTrigButtonImgs(screen)) {
    return INIT_FAILED;
  }

  if (!LoadStepButtonImgs(screen)) {
    return INIT_FAILED;
  }

  if (!LoadDigits(screen)) {
    return INIT_FAILED;
  }

  undo_button_surface =
    load_surface(screen, undo_button_inactive_file);
  redo_button_surface =
    load_surface(screen, redo_button_inactive_file);
  clear_button_surface =
    load_surface(screen, clear_button_inactive_file);

  if (!undo_button_surface || !redo_button_surface || !clear_button_surface) {
    return INIT_FAILED;
  }
}

SDLDrums::SDLDrums() {
  if (!InitSDL()) {
    CloseProgram();
  }

  const Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);
  Uint32 yellow = SDL_MapRGB(screen->format, 0xff, 0xb8, 0x2a);

  SDL_Surface* scope = SDL_CreateRGBSurface(SDL_SWSURFACE, 300, 200,
    screen->format->BitsPerPixel,
    screen->format->Rmask, screen->format->Gmask, screen->format->Bmask,
    screen->format->Amask);
  draw_border(screen, scope_rect);

  if (!sound_data.LoadSamples(samples_files)) {
    CloseProgram();
  }

  // Init drum loop and create sequencer
  drum_loop = std::make_unique<DrumLoop>(&sound_data);

  if (InitAllSurfaces(screen) == INIT_FAILED) {
    free_surfaces();
    CloseProgram();
  }

  // Init sound pads
  int button_pos_x = X_MARGIN;
  int button_pos_y = Y_MARGIN + 2 * SOUND_BUTTON_HEIGHT;
  for (unsigned i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    SDL_Rect rect;
    rect.x = button_pos_x;
    rect.y = button_pos_y;
    rect.w = SOUND_BUTTON_WIDTH;
    rect.h = SOUND_BUTTON_HEIGHT;
    sound_buttons[i] = std::make_unique<SoundButton>(
      screen, sound_buttons_inactive[i], sound_buttons_active[i], rect,
      sound_button_keys[i], &sound_data);
    sound_buttons[i]->Draw();

    button_pos_x += (SOUND_BUTTON_WIDTH);
    if ((i + 1) % 3 == 0) {
      button_pos_x = X_MARGIN;
      button_pos_y -= (SOUND_BUTTON_HEIGHT);
    }
  }

  // Init BPM buttons
  SDL_Rect bpm_rect;
  bpm_rect.x = scope_rect.x + 30;
  bpm_rect.y = Y_MARGIN;
  bpm_rect.w = bpm_up_10_inactive_surface->w;
  bpm_rect.h = bpm_up_10_inactive_surface->h;
  bpm_10_up_button = std::make_unique<Button>(
    screen, bpm_up_10_active_surface, bpm_up_10_inactive_surface, nullptr,
    bpm_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  bpm_10_up_button->Draw();

  bpm_rect.y += bpm_up_10_inactive_surface->h + 5;
  bpm_10_down_button = std::make_unique<Button>(
    screen, bpm_down_10_active_surface, bpm_down_10_inactive_surface, nullptr,
    bpm_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  bpm_10_down_button->Draw();

  bpm_rect.x += bpm_up_10_inactive_surface->w + 5;
  bpm_rect.y = Y_MARGIN;
  bpm_rect.w = bpm_up_1_inactive_surface->w;
  bpm_rect.h = bpm_up_1_inactive_surface->h;
  bpm_1_up_button = std::make_unique<Button>(
    screen, bpm_up_1_active_surface, bpm_up_1_inactive_surface, nullptr,
    bpm_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  bpm_1_up_button->Draw();

  bpm_rect.y += bpm_up_1_inactive_surface->h + 5;
  bpm_1_down_button = std::make_unique<Button>(
    screen, bpm_down_1_active_surface, bpm_down_1_inactive_surface, nullptr,
    bpm_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  bpm_1_down_button->Draw();

  bpm_indicator_rect =
  { bpm_rect.x + 60, bpm_rect.y - 20, bpm_rect.w, bpm_rect.h };

  bpm_rect.x += bpm_up_1_active_surface->w + 10;
  bpm_rect.y = Y_MARGIN;
  SDL_BlitSurface(bpm_empty_surface, nullptr, screen, &bpm_rect);

  DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());

  SDL_Rect trig_rect;
  trig_rect.x = X_MARGIN;
  trig_rect.y = SCREEN_HEIGHT - Y_MARGIN;
  trig_rect.w = empty_slot_surface->w;
  trig_rect.h = empty_slot_surface->h;
  for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
    trig_rect.x = X_MARGIN;
    trig_rect.y -= 27;
    for (int j = 0; j < STEPS_TOTAL; j++) {
      trig_buttons[i][j] = std::make_unique<TrigButton>(
        screen, active_empty_slot_surface, empty_slot_surface,
        trig_button_icons[i], trig_rect, drum_loop.get());

      // TODO: A little counter-intuitive having to do this.
      trig_buttons[i][j]->SetInactive();

      if (drum_loop->GetTrig(i, j) != '0') {
        trig_buttons[i][j]->Enable();
      }
      trig_buttons[i][j]->SetTrack(i);
      trig_buttons[i][j]->SetStep(j);
      trig_buttons[i][j]->Draw();

      trig_rect.x += 27;
      if ((j + 1) % 4 == 0) {
        if ((j + 1) % 16 == 0) {
          trig_rect.x += 15;
        }
        trig_rect.x += 8;
      }
    }
  }

  // Step buttons
  SDL_Rect step_rect;
  step_rect.x = X_MARGIN + 5;
  step_rect.y = trig_rect.y - step_button_icons[0]->h - 5;
  step_rect.w = step_button_icons[0]->w;
  step_rect.h = step_button_icons[0]->h;
  for (int i = 0; i < STEP_BUTTONS_TOTAL; i++) {
    step_buttons[i] = std::make_unique<StepButton>(screen, step_button_icons[i],
      step_rect, i + 1);
    step_buttons[i]->Draw();

    step_rect.x += 27;
    if ((i + 1) % 4 == 0) {
      if ((i + 1) % 16 == 0) {
        step_rect.x += 15;
      }
      step_rect.x += 8;
    }
  }

  // Control buttons
  SDL_Rect play_rect = { X_MARGIN, SCREEN_HEIGHT - 420,
      CONTROL_BUTTON_WIDTH, CONTROL_BUTTON_HEIGHT };
  play_button = std::make_unique<ControlButton>(
    screen, play_button_active_surface, play_button_inactive_surface,
    stop_button_surface, play_rect, SDLK_UNKNOWN, SDLK_UNKNOWN);
  play_button->Draw();

  SDL_Rect pause_rect = {
      play_rect.x + CONTROL_BUTTON_WIDTH, SCREEN_HEIGHT - 420,
      CONTROL_BUTTON_WIDTH, CONTROL_BUTTON_HEIGHT };
  pause_button = std::make_unique<ControlButton>(
    screen, nullptr, pause_button_surface,
    pause_button_toggled_surface, pause_rect, SDLK_p, SDLK_UNKNOWN);
  pause_button->Draw();

  SDL_Rect rec_rect;
  rec_rect.x = pause_rect.x + CONTROL_BUTTON_WIDTH;
  rec_rect.y = SCREEN_HEIGHT - 420;
  rec_rect.w = CONTROL_BUTTON_WIDTH;
  rec_rect.h = CONTROL_BUTTON_HEIGHT;
  rec_button = std::make_unique<ControlButton>(
    screen, nullptr, rec_button_surface,
    stop_button_surface, rec_rect, SDLK_r, SDLK_UNKNOWN);
  rec_button->Draw();

  // Edit buttons
  SDL_Rect edit_rect;
  edit_rect.x = SCREEN_WIDTH - undo_button_surface->w - 70;
  edit_rect.y = Y_MARGIN;
  edit_rect.w = 200;
  edit_rect.h = 50;
  undo_button = std::make_unique<Button>(screen, nullptr,
    undo_button_surface, undo_button_surface, edit_rect,
    SDLK_u, SDLK_UNKNOWN);
  undo_button->Draw();

  edit_rect.y += 10 + redo_button_surface->h;
  redo_button = std::make_unique<Button>(screen, nullptr,
    redo_button_surface, redo_button_surface, edit_rect,
    SDLK_r, SDLK_UNKNOWN);
  redo_button->Draw();

  edit_rect.y += 10 + clear_button_surface->h;
  edit_rect.w = 200;
  edit_rect.h = 50;
  clear_button = std::make_unique<Button>(screen, nullptr,
    clear_button_surface, clear_button_surface, edit_rect,
    SDLK_l, SDLK_UNKNOWN);
  clear_button->Draw();

  SDL_UpdateWindowSurface(window);
  Mix_SetPostMix(MixFunc, scope);
}

SDLDrums::~SDLDrums() {
  free_surfaces();
  CloseProgram();
}

int SDLDrums::Run() {
  // Main event loop
  SDL_Event e;
  bool quit = false;
  next_time = SDL_GetTicks() + TICK_INTERVAL;
  int current_step = -1;
  bool screen_needs_update;

  while (quit == false) {
    screen_needs_update = false;
    if (drum_loop->Running()) {
      int step = drum_loop->CurrentStep();
      if (step != current_step) {
        screen_needs_update = UpdateTrigs();
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
        if (drum_loop->Running()) {
          if (drum_loop->Recording()) {
            play_button->SetToggled(true);
            rec_button->SetToggled(false);
            pause_button->SetToggled(false);
            drum_loop->SetRec(false);
          }
          else {
            drum_loop->Stop();
            UpdateTrigs();
            play_button->SetToggled(false);
            pause_button->SetToggled(false);
          }
        }
        else {
          if (drum_loop->Paused() && !drum_loop->RecMode()) {
            drum_loop->Stop();
            play_button->SetToggled(false);
            pause_button->SetToggled(false);
            UpdateTrigs();
          }
          else {
            drum_loop->Start();
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
        if (drum_loop->Running()) {
          if (!drum_loop->RecMode()) {
            play_button->SetToggled(false);
            rec_button->SetToggled(true);
            drum_loop->SetRec(true);
          }
          else {
            rec_button->SetToggled(false);
            drum_loop->Stop();
            UpdateTrigs();
          }
        }
        else {
          if (drum_loop->Paused() && drum_loop->RecMode()) {
            play_button->SetToggled(false);
            rec_button->SetToggled(false);
            pause_button->SetToggled(false);
            drum_loop->Stop();
            UpdateTrigs();
          }
          else {
            play_button->SetToggled(false);
            drum_loop->StartWithRec();
            rec_button->SetToggled(true);
            pause_button->SetToggled(false);
          }
        }
      }

      bool pause_clicked = false;
      screen_needs_update |=
        pause_button->HandleEvent(&e, &pause_clicked);
      if (pause_clicked) {
        if (drum_loop->Running()) {
          drum_loop->Pause();
          UpdateTrigs();
          pause_button->SetToggled(true);
        }
        else {
          if (drum_loop->Paused()) {
            if (drum_loop->RecMode()) {
              drum_loop->StartWithRec();
              pause_button->SetToggled(false);
            }
            else {
              drum_loop->Start();
              pause_button->SetToggled(false);
            }
          }
        }
      }
      ////////////////////////////////

      bool bpm_10_up_clicked = false;
      bool bpm_1_up_clicked = false;
      bool bpm_10_down_clicked = false;
      bool bpm_1_down_clicked = false;
      screen_needs_update = bpm_10_up_button->HandleEvent(&e, &bpm_10_up_clicked);
      screen_needs_update = bpm_1_up_button->HandleEvent(&e, &bpm_1_up_clicked);
      screen_needs_update = bpm_10_down_button->HandleEvent(&e, &bpm_10_down_clicked);
      screen_needs_update = bpm_1_down_button->HandleEvent(&e, &bpm_1_down_clicked);

      if (bpm_10_up_clicked) {
        drum_loop->SpeedUp(10);
        DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
      }
      else if (bpm_1_up_clicked) {
        drum_loop->SpeedUp(1);
        DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
      }
      else if (bpm_10_down_clicked) {
        drum_loop->SpeedUp(-10);
        DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
      }
      else if (bpm_1_down_clicked) {
        drum_loop->SpeedUp(-1);
        DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
      }

      bool mousedown = false;
      bool clear_button_clicked = false;
      clear_button->HandleEventBase(&e, &mousedown, &clear_button_clicked);
      if (clear_button_clicked) {
        // Currently we just do this so that Clear will be added to the undo
        // list. The SetEnabled's on the trigs will also zero ou the drum loop.
        // Revisit this.
        drum_loop->ClearPattern();

        for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
          for (int j = 0; j < STEPS_TOTAL; j++)
            trig_buttons[i][j]->SetEnabled(false, false);
        }
        UpdateTrigs();
        screen_needs_update = true;
      }

      bool undo_button_clicked = false;
      undo_button->HandleEventBase(&e, &mousedown,
        &undo_button_clicked);
      if (undo_button_clicked) {
        DrumLoop::UndoAction action = drum_loop->Undo();
        ApplyUndoAction(action, true);
        screen_needs_update = true;
      }

      bool redo_button_clicked = false;
      redo_button->HandleEventBase(&e, &mousedown, &redo_button_clicked);
      if (redo_button_clicked) {
        DrumLoop::UndoAction action = drum_loop->Redo();
        ApplyUndoAction(action, false);
        screen_needs_update = true;
      }

      for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
        bool clicked = false;
        screen_needs_update |=
          sound_buttons[i]->HandleEvent(&e, &clicked);
        if (clicked && (drum_loop->Recording() || drum_loop->Paused())) {
          // TODO: Oh, boy is this a mess...
          int step = drum_loop->CurrentStep();
          bool erase = SDL_GetModState() & KMOD_SHIFT;
          if (erase) {
            trig_buttons[i][step]->SetEnabled(false, true);
            trig_buttons[i][step]->UpdateStep();
          }
          else {
            trig_buttons[i][step]->SetActive();
            trig_buttons[i][step]->SetEnabled(true, true);
            trig_buttons[i][step]->Draw();
          }
        }
      }
      for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
        for (int j = 0; j < STEPS_TOTAL; j++) {
          screen_needs_update |=
            trig_buttons[i][j]->HandleEvent(&e);
        }
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
          if (drum_loop->Running()) {
            drum_loop->Stop();
            screen_needs_update |= UpdateTrigs();
            play_button->SetToggled(false);
            rec_button->SetToggled(false);
          }
          else {
            if (drum_loop->RecMode() && drum_loop->Paused()) {
              play_button->SetToggled(false);
              rec_button->SetToggled(true);
              drum_loop->StartWithRec();
            }
            else {
              play_button->SetToggled(true);
              rec_button->SetToggled(false);
              drum_loop->Start();
            }
            pause_button->SetToggled(false);
          }
          break;
        case SDLK_ESCAPE:
          quit = true;
          break;
        case SDLK_UP:
          drum_loop->SpeedUp(10);
          DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
          break;
        case SDLK_DOWN:
          drum_loop->SlowDown(10);
          DrawBPM(screen, bpm_indicator_rect, drum_loop->GetBPM());
          break;
        case SDLK_RIGHT:
          drum_loop->NextStep();
          if (!drum_loop->Running()) {
            if (drum_loop->CurrentStep() != -1) {
              drum_loop->SetEditMode(true);
            }
            else {
              drum_loop->SetEditMode(false);
            }
          }
          screen_needs_update = UpdateTrigs();
          break;
        case SDLK_LEFT:
          drum_loop->PrevStep();
          if (!drum_loop->Running()) {
            if (drum_loop->CurrentStep() != -1) {
              drum_loop->SetEditMode(true);
            }
            else {
              drum_loop->SetEditMode(false);
            }
          }
          screen_needs_update = UpdateTrigs();
          break;
        case SDLK_b:
          printf("%i\n", drum_loop->CurrentStep());
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
  return 0;
}

int main(int argc, char* argv[]) {
  SDLDrums app;
  return app.Run();
}
