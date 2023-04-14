#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Condition bits
struct conditions {
    uint8_t carry : 1;
    uint8_t sign : 1;
    uint8_t zero : 1;
    uint8_t parity : 1;
    uint8_t aux_carry : 1;
    uint8_t padding : 3;
};

typedef struct conditions conditions;

struct cpu {
    // Registers
    uint8_t A;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;
    uint16_t SP;
    uint16_t PC;

    // Memory
    uint8_t* memory;

    // Condition bits
    conditions cond;
};

typedef struct cpu cpu;

void emulate(cpu* state) {

}

cpu* init_cpu(void) {

    cpu* state = malloc(sizeof(cpu));
    
    state->A = 0x00;
    state->B = 0x00;
    state->C = 0x00;
    state->D = 0x00;
    state->E = 0x00;
    state->H = 0x00;
    state->L = 0x00;

    state->PC = 0x0000;
    state->SP = 0x0000;

    //allocate 64k (65536 bytes)
    state->memory = (uint8_t*)malloc(sizeof(uint8_t) * 0xffff);

    state->cond.aux_carry = 0;
    state->cond.carry = 0;
    state->cond.parity = 0;
    state->cond.sign = 0;
    state->cond.zero = 0;

    return state;
}

int main(int argc, char** argv) {

    cpu* state = init_cpu();

    return 0;
}
