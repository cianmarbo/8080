#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rom.h"

int file_size = 0;

uint8_t* open_rom(char* fileName) {

	FILE* rom_file = fopen(fileName, "r");

	if(!rom_file) {
		printf("please point to a valid ROM...\n");
		exit(0);
	}

	//set the file position indicator to the end of the file (measured in bytes)
	fseek(rom_file, 0, SEEK_END);
	//get the file position indicator to store the size of the file in bytes
	file_size = ftell(rom_file);
	//set the file position indicator to the beginning of the file
	rewind(rom_file);

	uint8_t* rom_buffer = (uint8_t*) malloc(file_size);

	fread(rom_buffer, file_size, 1, rom_file);
	fclose(rom_file);

	return rom_buffer;
}

void write_rom(cpu* state, uint8_t* rom_buffer, int file_size) {
    static int memory_offset = 0;
    for (int i = 0; i < file_size; i++) {
        state->memory[memory_offset+i] = rom_buffer[i];
        if (!(state->memory[memory_offset+i] == rom_buffer[i])) {
            fprintf(stderr, "There was an error writing the ROM");
        }
    }

    memory_offset += file_size;
}
