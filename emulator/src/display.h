#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <stdbool.h>

#include "cpu.h"

#define DISP_SCALE          1
#define WINDOW_HEIGHT       (224 * DISP_SCALE)
#define WINDOW_WIDTH        (256 * DISP_SCALE)
#define HOST_PIXEL_STRIDE   4

#define FRAME_BUFFER_SIZE_BYTES (WINDOW_HEIGHT * WINDOW_WIDTH * HOST_PIXEL_STRIDE)

#define REFRESH_RATE    60
#define CPU_CLOCK       2000000

#define VBLANK_RATE     CPU_CLOCK / REFRESH_RATE //how many CPU cycles a frame takes up (CPU cyles per frame)

// Set to false by process_input() when the OS asks the window to close;
// the main loop polls it to decide when to stop.
extern bool running;

bool init_window(void);
void clear_framebuffer(void);
void render_framebuffer(void);
void process_input(void);
void destroy_window(void);
void quit(void);
void draw_pixel(uint32_t posX, uint32_t posY, uint32_t colour);
void render(cpu* state);

#endif
