#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"
#include "display.h"
#include "rom.h"

const char* version_string = "0.0.3";
const char* build_date = __DATE__;

#define MAX_INTRO_LINES 14
#define MAX_INTRO_CHARS 50

void about_info(void) {
    printf("version: %s\n", version_string);
    printf("build date: %s\n", build_date);
}

// Check command line arguments

int check_args(char** argv) {
    if(strcmp(argv[1], "--dumpregisters") == 0) {
        return 0;
    }

    if(strcmp(argv[1], "--about") == 0) {
        return 1;
    }

    return -1;
}

void handle_args(int argc, char** argv, cpu* state) {

    if(argc >= 2) {
        
        if(check_args(argv) == 0) {
            dump_registers(state);
        }

        if(check_args(argv) == 1) {
            about_info();
        }
    }
}

void display_intro() {

	char intro_array[MAX_INTRO_LINES][MAX_INTRO_CHARS];
	int intro_array_count = 0;

	FILE* intro_file = fopen("intro.txt", "r");

	if(intro_file) {
		while(fgets(intro_array[intro_array_count], MAX_INTRO_CHARS, intro_file) != NULL) {
			intro_array_count++;
			
			if(intro_array_count == MAX_INTRO_LINES) {
				break;
			}
		}

		for(int i = 0; i < intro_array_count; i++) {
			printf("%s", intro_array[i]);
		}
	}

	fclose(intro_file);
}

int main(int argc, char** argv) {

    //display_intro();

    cpu* state = init_cpu();

    // test(state);

    uint8_t* rom_1 = open_rom(argv[1]);
    write_rom(state, rom_1, file_size);

    uint8_t* rom_2 = open_rom(argv[2]);
    write_rom(state, rom_2, file_size);

    uint8_t* rom_3 = open_rom(argv[3]);
    write_rom(state, rom_3, file_size);

    uint8_t* rom_4 = open_rom(argv[4]);
    write_rom(state, rom_4, file_size);

    // for (int i = 0; i < 8196; i++) {
    //     printf("%02x\n", state->memory[i]);
    // }

    if (!init_window()) {
        return 1;
    }
    running = true;

    while(running) {
        process_input();

        // Start each frame's cycle budget from zero so the interrupts below
        // fire at the same beam positions every frame.
        state->total_cpu_cycles = 0;

        // below we're basically using the CPU clock as a way to measure time
        // the VBLANK_RATE is how many CPU cycles it takes to render a frame
        // typically the display hardware in the arcade cabinet would be setup to send the interrupt every half scan
        // obviously we don't have a real display, so we simulate that by allowing the CPU to run what time it takes a half a frame to be rendered
        // we do that by checking if the number of CPU cycles executed so far is less than the half the amount of cycles it takes to render 1 frame

        // Run the first half of the frame, then the mid-screen interrupt.
        while (state->total_cpu_cycles < (VBLANK_RATE / 2)) {
            execute(state);
        }
        generate_interrupt(state, 1);   // RST 1 -> 0x08 (mid-screen)

        // Run the rest of the frame, then the VBlank interrupt.
        while (state->total_cpu_cycles < VBLANK_RATE) {
            execute(state);
        }
        generate_interrupt(state, 2);   // RST 2 -> 0x10 (VBlank)

        render(state);
    }

    handle_args(argc, argv, state);

    quit();

    return 0;
}
