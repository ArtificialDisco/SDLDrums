#ifndef DRUM_LOOP_H
#define DRUM_LOOP_H

#include <vector>

#include "sdl_drums.h"
#include "sound_data.h"

#define TRACK_MAX 1000

#define MAIN_PATTERN_FILE "./patterns/main.txt"
#define MAX_UNDO 1000

class DrumLoop
{
 public:
  DrumLoop(SoundData* sound_data);
  ~DrumLoop();

  enum ActionType {
    None = 0,
    TrigEdit,
    ClearAll,
  };

  struct TrigEntry {
    int track;
    int step;
    char data;
  };

  struct UndoAction {
    ActionType type;
    void* data;
  };

  struct Pattern {
    char tracks[9][33];
  };

  void WritePatternToFile(const char* file);
  void Start();
  void StartWithRec();
  void SetRec(bool rec);
  bool Recording();
  bool RecMode();
  void Stop();
  void Pause();
  bool Paused();
  int GetBPM() { return bpm_; } 
  void SetBPM(int bpm);
  void SpeedUp(int bpm);
  void SlowDown(int bpm);
  void NextStep();
  void PrevStep();
  char GetTrig(int track, int step);
  void SetTrig(int track, int step, char data, bool undoable = true);
  UndoAction Undo();
  UndoAction Redo();
  int CurrentStep();
  void ClearPattern();
  void Init();
  
  bool Running() { return loop_running_; }
  
  int LoopFunc(void* thread_data);

  void PrintUndoEntries();

 private:
  void CopyPattern(Pattern *to, Pattern *from);
  bool IsPatternEmpty(Pattern *p);
  void ShrinkUndoListIfNeeded();
  UndoAction ApplyUndoAction();

  std::vector<UndoAction> undo_list;
  int current_undo = 0;

  SoundData* sound_data_;
  SDL_Thread* loop_thread_;

  bool loop_running_ = false;
  int current_step_ = STOPPED;
  bool rec_mode_ = false;
  bool paused_ = false;

  int bpm_ = 120;

  Pattern main_pattern_;
};

#endif  // DRUM_LOOP_H
