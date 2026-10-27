#ifndef _INPUT_H
#define _INPUT_H

#include <SDL2/SDL.h>

#include "cpu.h"

// Open/close the controller when one is plugged in or removed.
// Call this for every SDL event; it ignores the ones it doesn't care about.
void input_handle_event(SDL_Event* event);

// Read the keyboard and controller, and set the cabinet's input ports to match.
// Call this once per frame.
void update_input(cpu* state);

#endif
