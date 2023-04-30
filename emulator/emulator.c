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

void execute(cpu* state) {
    
    uint8_t* instruction = state->memory[state->PC];

    switch(*instruction) {
        case 0x00:
            state->PC += 1;
            break;
        case 0x01:
            break;
        //BEGIN ADDITION GROUP
        case 0x80:
            //ADD B
            uint16_t result = (uint16_t)state->A + (uint16_t)state->B;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            // assign result by doing bitwise AND on 0xff (255 base 10) to prevent overflow
            state->A = result & 0xff;
            break;
        case 0x81:
            // ADD C
            uint16_t result = (uint16_t)state->A + (uint16_t)state->C;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x82:
            //ADD D
            uint16_t result = (uint16_t)state->A + (uint16_t)state->D;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x83:
            //ADD E
            uint16_t result = (uint16_t)state->A + (uint16_t)state->E;
            
            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x84:
            //ADD H
            uint16_t result = (uint16_t)state->A + (uint16_t)state->H;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x85:
            //ADD L
            uint16_t result = (uint16_t)state->A + (uint16_t)state->L;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x86:
            //ADD M (memory address referenced by combo of H and L)
            uint16_t memory_offset = (state->H << 8) + state->L;
            uint16_t result = (uint16_t)state->A + (uint16_t)state->memory[memory_offset];

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x87:
            // ADD A
            uint16_t result = (uint16_t)state->A + (uint16_t)state->A;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x88:
            //ADC B
            uint16_t result = (uint16_t)state->A + (uint16_t)state->B + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x89:
            //ADC C
            uint16_t result = (uint16_t)state->A + (uint16_t)state->C + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8A:
            //ADC D
            uint16_t result = (uint16_t)state->A + (uint16_t)state->D + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8B:
            //ADC E
            uint16_t result = (uint16_t)state->A + (uint16_t)state->E + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8C:
            //ADC H
            uint16_t result = (uint16_t)state->A + (uint16_t)state->H + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8D:
            //ADC L
            uint16_t result = (uint16_t)state->A + (uint16_t)state->L + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8E:
            //ADC M (memory address made up of H + L combo)
            uint16_t memory_offset = (state->H << 8) + state->L;
            uint16_t result = (uint16_t)state->A + (uint16_t)state->memory[memory_offset] + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8F:
            //ADC A
            uint16_t result = (uint16_t)state->A + (uint16_t)state->A + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        //BEGIN SUBTRACTION GROUP
        case 0x90:
            uint16_t result = (uint16_t)state->A - (uint16_t)state->B;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            

        default:
            break;
    }
}

//Stack allocated

struct Window {

};


struct Window my_window;

struct Window* create_window() {


    return &my_window;
}

void caller() {

    struct Window* window = create_window();
}

//

//Heap allocated

struct Window2 {

};

struct Window2* create_window2() {

    struct Window* window = malloc(sizeof(struct Window2));

    return window;
}

void caller2() {

    struct Window* window = create_window2();
}

//

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
