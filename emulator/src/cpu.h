#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

#define PSW_FLAG        1

// Condition bits
struct flags {
    uint8_t carry       : 1;
    uint8_t sign        : 1;
    uint8_t zero        : 1;
    uint8_t parity      : 1;
    uint8_t aux_carry   : 1;
    uint8_t             : 0;
};

typedef struct flags flags;

typedef struct {
    uint8_t INTE        : 1;    // INTE is name of 8080's "Interrupt Enable" bit
} interrupt;

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

    interrupt interrupt_flag;

    // Condition bits
    flags cond;

    uint32_t total_cpu_cycles;
};

typedef struct cpu cpu;

uint8_t calculate_parity(uint8_t value);

cpu* init_cpu(void);
void execute(cpu* state);
void generate_interrupt(cpu* state, uint8_t interrupt_num);
void dump_registers(cpu* state);
void test(cpu* state);

#endif
