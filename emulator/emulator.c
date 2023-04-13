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
    uint8_t padding : 1;
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
    conditions c;
};

typedef struct cpu cpu;

int main(int argc, char** argv) {

    return 0;
}