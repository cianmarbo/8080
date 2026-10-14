#ifndef _CRT_H
#define _CRT_H

#include <stdbool.h>
#include <stdint.h>

#include <SDL2/SDL.h>

// Tuning values for the CRT effect (all passed to the shaders as uniforms).
#define CRT_PERSISTENCE 0.55f   // how much of the previous frame lingers (0 = none, 1 = never fades)
#define CRT_MARGIN      0.05f   // empty space around the picture, as a fraction of the window
#define CRT_CURVATURE   0.10f   // how much the edges of the picture bulge outwards
#define CRT_CORNER      0.08f   // roundness of the picture's corners
#define CRT_BLOOM       0.35f   // strength of the glow around bright pixels
#define CRT_MASK        0.10f   // strength of the faint RGB stripe pattern
#define CRT_VIGNETTE    0.35f   // how much the edges are darkened

void crt_set_gl_attributes(void);               // call before SDL_CreateWindow
bool crt_init(SDL_Window* window);              // call once the window (with SDL_WINDOW_OPENGL) exists
void crt_draw(const uint32_t* pixels);          // shows one GAME_WIDTH x GAME_HEIGHT ARGB frame
void crt_destroy(void);

#endif
