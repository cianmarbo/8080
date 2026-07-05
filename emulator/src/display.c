#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "display.h"

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
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    if (!texture) {
        fprintf(stderr, "There was a problem creating an SDL texture...\n");
        return false;
    }

    frame_buffer = (uint32_t*)malloc(FRAME_BUFFER_SIZE_BYTES);

    return true;
}

void clear_framebuffer(void) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        frame_buffer[i] = 0xFF000000;
    }
}

void render_framebuffer(void) {

    SDL_UpdateTexture(texture, NULL, frame_buffer, WINDOW_WIDTH * HOST_PIXEL_STRIDE);

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
    if (posX < WINDOW_WIDTH && posY < WINDOW_HEIGHT) {
        frame_buffer[(WINDOW_WIDTH * posY) + posX] = colour;
    }
}

void render(cpu* state) {
    clear_framebuffer();

    uint16_t vid_mem_index = 0x2400;

    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x+=8) {
            for (int bit = 0; bit < 8; bit++) {
                frame_buffer[(y * WINDOW_WIDTH) + (x + bit)] = ((state->memory[vid_mem_index] >> bit) & 0x1) == 1 ? 0xFFFFFFFF: 0xFF000000;
                //frame_buffer[(y * WINDOW_WIDTH) + (x + bit)] = ((state->memory[vid_mem_index] >> bit) & 0x1) == 1 ? 0xFF000000 : 0xFFFFFFFF;
            }
            vid_mem_index++;
        }
    }

    render_framebuffer();
}
