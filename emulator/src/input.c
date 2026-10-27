#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "input.h"

// How far a stick or trigger has to be pushed (out of 32767) to count as pressed
#define AXIS_THRESHOLD  16384

// Bits that are always set on the cabinet's input ports
#define PORT0_FIXED     0x0E
#define PORT1_FIXED     0x08

// DIP switch settings, read through port 2: 3 lives, extra ship at 1500, coin info shown
#define PORT2_DIP_SETTINGS  0x00

// Bit positions in ports 1 and 2
#define BIT_COIN        0   // port 1 only
#define BIT_START_2P    1   // port 1 only
#define BIT_START_1P    2   // port 1 only
#define BIT_FIRE        4
#define BIT_LEFT        5
#define BIT_RIGHT       6

// The controller we're reading from, or NULL if none is plugged in.
// Only one is used at a time; any others are ignored.
static SDL_GameController* controller = NULL;

void input_handle_event(SDL_Event* event) {
    switch (event->type) {
        case SDL_CONTROLLERDEVICEADDED:
            // here "which" is the device's index
            if (!controller) {
                controller = SDL_GameControllerOpen(event->cdevice.which);
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            // here "which" is the instance id of the controller that was unplugged
            if (controller && event->cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
                SDL_GameControllerClose(controller);
                controller = NULL;
            }
            break;
    }
}

void update_input(cpu* state) {
    const uint8_t* keys = SDL_GetKeyboardState(NULL);

    bool coin     = keys[SDL_SCANCODE_5];
    bool start_1p = keys[SDL_SCANCODE_1];
    bool start_2p = keys[SDL_SCANCODE_2];
    bool left     = keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT];
    bool right    = keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT];
    bool fire     = keys[SDL_SCANCODE_SPACE];

    if (controller) {
        int stick_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);

        left     |= SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || stick_x < -AXIS_THRESHOLD;
        right    |= SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || stick_x > AXIS_THRESHOLD;
        // RT on an Xbox controller, R2 on a PlayStation one. The trigger is an axis, not a button.
        fire     |= SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > AXIS_THRESHOLD;
        coin     |= SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK);
        start_1p |= SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
    }

    // The same inputs are used for both players; only one of them is playing at a time
    uint8_t player = (fire << BIT_FIRE) | (left << BIT_LEFT) | (right << BIT_RIGHT);

    state->in_ports[0] = PORT0_FIXED;
    state->in_ports[1] = PORT1_FIXED | player | (coin << BIT_COIN) | (start_2p << BIT_START_2P) | (start_1p << BIT_START_1P);
    state->in_ports[2] = PORT2_DIP_SETTINGS | player;
}
