#ifndef SDL_DRUMS_H
#define SDL_DRUMS_H

// State of drum machine
const int STOPPED = -1;

// Error codes
const int INIT_FAILED = 1;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const int SOUND_BUTTONS_TOTAL = 9;
const int STEP_BUTTONS_TOTAL = 8;
//const int TRIG_BUTTONS_TOTAL = 9*32;
const int STEPS_TOTAL = 32;
const int TICK_INTERVAL = 5;

const int SOUND_BUTTON_WIDTH = 82;
const int SOUND_BUTTON_HEIGHT = 82;

const int CONTROL_BUTTON_WIDTH = 82;
const int CONTROL_BUTTON_HEIGHT = 82;

#endif  // SDL_DRUMS_H
