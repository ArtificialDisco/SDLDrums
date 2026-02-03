#include <functional>
#include <fstream>
#include <thread>
#include <chrono>

#include "drum_loop.h"

//Redefintion that lets us skip including sdl_drums.h
// Need to find a better place for this.
const int SOUND_BUTTONS_TOTAL = 9;

DrumLoop::DrumLoop(SoundData* sound_data) {
  sound_data_ = sound_data;
  std::fstream stream;
  stream.open(MAIN_PATTERN_FILE, std::ios_base::in);
  char arr[100];
  if (stream.is_open()) {
    for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
      stream.getline(arr, 100, '\n');
      arr[32] = '\0';
      strncpy(main_pattern_.tracks[i], arr, 32);
      main_pattern_.tracks[i][32] = '\0';
    }
    stream.close();
  } else {
    printf("Couldn't open main pattern file %s\n", MAIN_PATTERN_FILE);
    ClearPattern();
  }
  /*for (int i = 0; i < MAX_UNDO; i++) {
    undo_list[i].type = None;
    undo_list[i].data = nullptr;
  }*/
}

DrumLoop::~DrumLoop() {
  if (loop_running_) {
    Stop();
  }
  WritePatternToFile(MAIN_PATTERN_FILE);
  for (int i = 0; i < undo_list.size(); i++) {
    if (undo_list[i].data != nullptr) {
      delete undo_list[i].data;
    }
  }
}

static int StaticLoopFunc(void* drum_loop_object) {
  return ((DrumLoop*)drum_loop_object)->LoopFunc(drum_loop_object);
}

void DrumLoop::WritePatternToFile(const char* filename) {
  std::fstream stream;
  stream.open(filename, std::ios_base::out);
  if (stream.is_open()) {
    for (int i = 0; i < SOUND_BUTTONS_TOTAL; i++) {
      stream.write(main_pattern_.tracks[i], 32);
      stream.write("\n", 1);
    }
    stream.close();
  }
}

void DrumLoop::Start() {
  loop_running_ = true;
  paused_ = false;
  rec_mode_ = false;
  //if (!loop_thread_)
    loop_thread_ = SDL_CreateThread(StaticLoopFunc, "Hello!", this);
}

void DrumLoop::SetRec(bool rec) {
  rec_mode_ = rec;
}

void DrumLoop::StartWithRec() {
  loop_running_ = true;
  paused_ = false;
  rec_mode_ = true;
  //if (!loop_thread_)
    loop_thread_ = SDL_CreateThread(StaticLoopFunc, "Hello!", this);
}

bool DrumLoop::Recording() {
  return Running() && RecMode();
}

bool DrumLoop::RecMode() {
  return rec_mode_;
}

void DrumLoop::Stop() {
  loop_running_ = false;
  paused_ = false;
  current_step_ = STOPPED;
  //SDL_WaitThread(loop_thread_, NULL);
}

void DrumLoop::Pause() {
  loop_running_ = false;
  //rec_mode_ = false;
  paused_ = true;
  SDL_WaitThread(loop_thread_, NULL);
}

bool DrumLoop::Paused() {
  return paused_;
}

void DrumLoop::SetBPM(int bpm) {
  bpm_ = bpm;
}

void DrumLoop::SpeedUp(int bpm) {
  bpm_ += bpm;
}

void DrumLoop::SlowDown(int bpm) {
  if (bpm_ - bpm > 0) {
    bpm_ -= bpm;
  }
}

void DrumLoop::NextStep() {
  if (current_step_ < 31) {
    current_step_++;
  }
}

void DrumLoop::PrevStep() {
  if (current_step_ >= 0)
    current_step_--;
}

char DrumLoop::GetTrig(int track, int step) {
  return main_pattern_.tracks[track][step];
}

void DrumLoop::SetTrig(int track, int step, char data, bool undoable) {
  if (undoable) {
    TrigEntry *entry = new TrigEntry { track, step, data };
    UndoAction action;
    action.type = TrigEdit;
    action.data = entry;
    ShrinkUndoListIfNeeded();
    undo_list.push_back(action);
    current_undo++;
  }
  main_pattern_.tracks[track][step] = data;
}

void DrumLoop::PrintUndoEntries() {
  for (int i = 0; i < current_undo; i++) {
    if (undo_list[i].type == TrigEdit) {
      TrigEntry* entry = (TrigEntry*)(undo_list[i].data);
      printf("Entry %i = [%i %i %i]\n", i,
          entry->track, entry->step, entry->data);
    } else if (undo_list[i].type == ClearAll) {
      Pattern* p = (Pattern*)(undo_list[i].data);
      for (int i = 0; i < 9; i++) {
        printf("%s\n", p->tracks[i]);
      }
    }
  }
  printf("\n");
}

DrumLoop::UndoAction DrumLoop::Undo() {
  if (current_undo == 0) {
    UndoAction action = {None, nullptr};
    return action;
  }
  current_undo--;
  return ApplyUndoAction();
}

DrumLoop::UndoAction DrumLoop::Redo() {
  if (current_undo >= undo_list.size()) {
    UndoAction action = { None, nullptr };
    return action;
  }
  PrintUndoEntries();
  UndoAction apply = ApplyUndoAction();
  current_undo++;
  return apply;
}

// Used for both Undo and Redo. It's those function's responsibility to
// advance the current_undo value and then call this.
DrumLoop::UndoAction DrumLoop::ApplyUndoAction() {
  UndoAction action = undo_list[current_undo];
  if (action.type == TrigEdit) {
    TrigEntry* entry = (TrigEntry*)(action.data);
    int track = entry->track;
    int step = entry->step;
    char data = entry->data;
    main_pattern_.tracks[track][step] = data;
  } else if (action.type == ClearAll) {
    Pattern* p = (Pattern*)(action.data);
    CopyPattern(&main_pattern_, p);
  }
  return action;
}

void DrumLoop::SetEditMode(bool edit) {
  rec_mode_ = edit;
  paused_ = edit;
}

void DrumLoop::Init() {
  current_step_ = 0;
}

int DrumLoop::CurrentStep() {
  return current_step_;
}

void DrumLoop::ShrinkUndoListIfNeeded() {
  while (undo_list.size() > current_undo) {
    UndoAction* action = &undo_list.back();
    if (action->type == TrigEdit) {
      TrigEntry* e = (TrigEntry*)action->data;
      delete e;
    } else if (action->type == ClearAll) {
      Pattern* p = (Pattern*)action->data;
      delete p;
    }
    undo_list.pop_back();
  }
}

void DrumLoop::ClearPattern() {
  if (current_undo > 0 && undo_list[current_undo-1].type == ClearAll) {
    printf("Already Cleared\n");
    return;
  }
  if (IsPatternEmpty(&main_pattern_)) {
    printf("Already Empty\n");
    return;
  }

  UndoAction action;
  action.type = ClearAll;

  Pattern* p = new Pattern;
  CopyPattern(p, &main_pattern_);
  action.data = p;

  ShrinkUndoListIfNeeded();
  undo_list.push_back(action);

  for (int i = 0; i < 9; i++) {
    strncpy(main_pattern_.tracks[i], "00000000000000000000000000000000", 33);
  }
  current_undo++;
}

void DrumLoop::CopyPattern(Pattern* to, Pattern* from) {
  for (int i = 0; i < 9; i++) {
    strncpy(to->tracks[i], from->tracks[i], 33);
  }
}

bool DrumLoop::IsPatternEmpty(Pattern *p) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 32; j++) {
        if (p->tracks[i][j] != '0')
            return false;
    }
  }
  return true;
}

int DrumLoop::LoopFunc(void* thread_data) {
  int step_length = (int)(60000.0 / (bpm_ * 4.0));
  int nsamples = step_length*SampleRate * 2 / 1000;
  int channel = -1;
  int delay;
  int loop_length = 32;
  int tick = 10;

  Uint32 now = SDL_GetTicks();
  Uint32 next_time = now;

  // This needs to be accessed from outside the loop.
  int* step = &(((DrumLoop*)thread_data)->current_step_);
  //*step = -1;
  int last_bpm = bpm_;
  //while (true)
  while (loop_running_) {
    if (last_bpm != bpm_) {
      step_length = (int)(60000.0 / (bpm_* 4.0));
      nsamples = step_length * SampleRate * 2 / 1000;
      last_bpm = bpm_;
    }

    next_time += step_length;
    (*step)++;
    if (*step >= loop_length) {
      *step = 0;
    }

    if (!loop_running_) {
      return 0;
    }
    for (int i = 0; i < 9; i++) {
      if (main_pattern_.tracks[i][*step] != '0') {
        SoundData* sd = ((DrumLoop*)thread_data)->sound_data_;
        sd->PlaySample(i);
      }
    }
    
    if (!loop_running_) {
      return 0;
    }

    Uint32 now = SDL_GetTicks();
    delay = next_time - now;

    /*if (loop_running_) {
      SoundData** sd = &(((DrumLoop*)thread_data)->sound_data_);
      (*sd)->AdvanceDelayBuffer(nsamples);
      
    }*/
    if(delay > 0)
      SDL_Delay(delay);
  }
  return 0;
}
