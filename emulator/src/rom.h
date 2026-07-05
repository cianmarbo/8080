#ifndef _ROM_H
#define _ROM_H

#include <stdint.h>

#include "cpu.h"

// Size in bytes of the most recently opened ROM file, set by open_rom().
extern int file_size;

uint8_t* open_rom(char* fileName);
void write_rom(cpu* state, uint8_t* rom_buffer, int file_size);

#endif
