#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "display.h"
#include "logo_bitmap.h"   // LOGO_WIDTH, LOGO_HEIGHT and the logo[] pixel data

bool running = NULL;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

uint32_t* frame_buffer;

bool init_window(void) {

    if (SDL_InitSubSystem(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL failed to init...\n");
        return false;
    }

    window = SDL_CreateWindow(
        "8080",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window) {
        fprintf(stderr, "There was an error creating a window...\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "There was an error creating an SDL Renderer...\n");
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GAME_WIDTH,
        GAME_HEIGHT
    );

    if (!texture) {
        fprintf(stderr, "There was a problem creating an SDL texture...\n");
        return false;
    }

    frame_buffer = (uint32_t*)malloc(FRAME_BUFFER_SIZE_BYTES);

    return true;
}

void clear_framebuffer(void) {
    for (int i = 0; i < GAME_WIDTH * GAME_HEIGHT; i++) {
        frame_buffer[i] = 0xFF000000;
    }
}

void render_framebuffer(void) {

    SDL_UpdateTexture(texture, NULL, frame_buffer, GAME_WIDTH * HOST_PIXEL_STRIDE);

    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            running = false;
            break;
    }
}

void destroy_window(void) {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);

    SDL_Quit();
}

void quit(void) {
    destroy_window();
    free(frame_buffer);
}

void draw_pixel(uint32_t posX, uint32_t posY, uint32_t colour) {
    if (posX < GAME_WIDTH && posY < GAME_HEIGHT) {
        frame_buffer[(GAME_WIDTH * posY) + posX] = colour;
    }
}

void render(cpu* state) {
    clear_framebuffer();

    // VRAM is stored for a CRT mounted on its side: each byte is 8 vertically
    // stacked pixels, and 32 consecutive bytes make up one screen column.
    // So byte i is in column (i / 32), and holds rows (i % 32) * 8 .. +7 counted
    // from the bottom of the screen. Rotate while unpacking so it's upright.
    for (int i = 0; i < GAME_WIDTH * GAME_HEIGHT / 8; i++) {
        uint8_t byte = state->memory[0x2400 + i];
        int x = i / 32;
        for (int bit = 0; bit < 8; bit++) {
            int y = (GAME_HEIGHT - 1) - ((i % 32) * 8 + bit);
            frame_buffer[(y * GAME_WIDTH) + x] = ((byte >> bit) & 0x1) == 1 ? 0xFFFFFFFF : 0xFF000000;
        }
    }

    render_framebuffer();
}

// splash_screen - plays the intro logo animation before the game starts.
//
// The logo is a 1-bit-per-pixel bitmap baked into logo_bitmap.h (logo[y][x],
// where 1 means "lit"). We reveal it one vertical column at a time, left to
// right, so it appears to "load in" from the side, then hold the finished
// logo on screen for a moment before returning to main().
void splash_screen(void) {
    const uint32_t reveal_ms = 1200;   // how long the left-to-right wipe takes
    const uint32_t hold_ms   = 2000;   // how long to hold the full logo afterwards

    // Centre the logo inside the window (offsets are 0 when it fills the width).
    const int x_offset = (GAME_WIDTH  - LOGO_WIDTH)  / 2;
    const int y_offset = (GAME_HEIGHT - LOGO_HEIGHT) / 2;

    // We drive the animation off the wall clock, not a fixed step count, so it
    // runs for the same real duration regardless of how fast the machine loops.
    const uint32_t start = SDL_GetTicks();

    // --- Wipe-in phase -----------------------------------------------------
    while (running) {
        process_input();               // keep the window responsive / closeable
        if (!running) return;          // user closed the window during the intro

        // Convert elapsed time into a number of columns to reveal. At elapsed
        // == reveal_ms this reaches LOGO_WIDTH, i.e. the whole logo is shown.
        uint32_t elapsed = SDL_GetTicks() - start;
        int cols = (int)((elapsed * LOGO_WIDTH) / reveal_ms);
        if (cols > LOGO_WIDTH) cols = LOGO_WIDTH;

        // Redraw from scratch each frame: black background, then every lit
        // logo pixel that falls within the revealed columns.
        clear_framebuffer();
        for (int x = 0; x < cols; x++) {
            for (int y = 0; y < LOGO_HEIGHT; y++) {
                if (logo[y][x]) {
                    draw_pixel(x_offset + x, y_offset + y, 0xFFFFFFFF);
                }
            }
        }
        render_framebuffer();

        if (cols >= LOGO_WIDTH) break; // whole logo now visible -> stop wiping
        SDL_Delay(16);                 // ~60 fps pacing so the wipe looks smooth
    }

    // --- Hold phase --------------------------------------------------------
    // The full logo is already on screen, so we just wait, but keep pumping
    // input so the window doesn't appear frozen during the pause.
    const uint32_t hold_start = SDL_GetTicks();
    while (running && SDL_GetTicks() - hold_start < hold_ms) {
        process_input();
        SDL_Delay(16);
    }
}
