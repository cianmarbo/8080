#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_INTRO_LINES 14
#define MAX_INTRO_CHARS 50

uint8_t* register_array = NULL;

static int file_size;

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
    flags cond;
};

typedef struct cpu cpu;

static void ADD(cpu* state, uint8_t* operand) {

    uint16_t result = (uint16_t)state->A + (uint16_t)operand;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (result > 0xff) ? 1 : 0;

    // assign result by doing bitwise AND on 0xff (255 base 10) to prevent overflow
    state->A = result & 0xff;
}

static void ADC(cpu* state, uint8_t* operand) {

    uint16_t result = (uint16_t)state->A + (uint16_t)operand + state->cond.carry;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (result > 0xff) ? 1 : 0;

    state->A = result & 0xff;
}

static void SUB(cpu* state, uint8_t* operand) {

    uint16_t result = (uint16_t)state->A - (uint16_t)operand;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (result > 0xff) ? 0 : 1;

    state->A = result & 0xff;
}

void execute(cpu* state) {
    
    uint8_t* instruction = &state->memory[state->PC];

    uint16_t result;
    uint16_t memory_offset;

    printf("Instruction - %02x\n", *instruction);

    switch(*instruction) {
        case 0x00:
            // state->PC += 2;
            break;
        case 0x01:
            break;
        //BEGIN MOV GROUP
        case 0x40:
            //MOV B, B
            state->B = state->B;
            break;
        case 0x41:
            //MOV B, C
            state->B = state->C;
            break;
        case 0x42:
            state->B = state->D;
            break;
        case 0x43:
            state->B = state->E;
            break;
        case 0x44:
            state->B = state->H;
            break;
        case 0x45:
            state->B = state->L;
            break;
        case 0x46:
            memory_offset = (state->H) << 8 + state->L;
            state->B = state->memory[memory_offset];
            break;
        case 0x47:
            state->B = state->A;
            break;
        case 0x48:
            state->C = state->B;
            break;
        case 0x49:
            state->C = state->C;
            break;
        case 0x4A:
            state->C = state->D;
            break;
        case 0x4B:
            state->C = state->E;
            break;
        case 0x4C:
            state->C = state->H;
            break;
        //BEGIN ADDITION GROUP
        case 0x80:
            ADD(state, state->B);
            break;
        case 0x81:
            // ADD C
            ADD(state, state->C);
            break;
        case 0x82:
            //ADD D
            ADD(state, state->D);
            break;
        case 0x83:
            //ADD E
            ADD(state, state->E);
            break;
        case 0x84:
            //ADD H
            ADD(state, state->H);
            break;
        case 0x85:
            //ADD L
            ADD(state, state->L);
            break;
        case 0x86:
            //ADD M (memory address referenced by combo of H and L)
            memory_offset = (state->H << 8) + state->L;
            ADD(state, state->memory[memory_offset]);
            break;
        case 0x87:
            // ADD A
            ADD(state, state->A);
            break;
        case 0x88:
            //ADC B
            ADC(state, state->B);
            break;
        case 0x89:
            //ADC C
            ADC(state, state->C);
            break;
        case 0x8A:
            //ADC D
            ADC(state, state->D);
            break;
        case 0x8B:
            //ADC E
            ADC(state, state->E);
            break;
        case 0x8C:
            //ADC H
            ADC(state, state->H);
            break;
        case 0x8D:
            //ADC L
            ADC(state, state->L);
            break;
        case 0x8E:
            //ADC M (memory address made up of H + L combo)
            memory_offset = (state->H << 8) + state->L;
            ADC(state, state->memory[memory_offset]);
            break;
        case 0x8F:
            //ADC A
            ADC(state, state->A);
            break;
        //BEGIN SUBTRACTION GROUP
        case 0x90:
            //SUB B
            SUB(state, state->B);
            break;
        case 0x91:
            //SUB C
            SUB(state, state->C);
            break;
        case 0x92:
            //SUB D
            SUB(state, state->D);
            break;
        case 0x93:
            //SUB E
            SUB(state, state->E);
            break;
        case 0x94:
            //SUB H
            SUB(state, state->H);
            break;
        case 0x95:
            //SUB L
            SUB(state, state->L);
            break;
        case 0x96:
            //SUB M
            memory_offset = (state->H << 8) + state->L;
            SUB(state, state->memory[memory_offset]);
            break;
        case 0x97:
            //SUB A
            SUB(state, state->A);
            break;
        default:
            break;
    }
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

    state->SP = 0xeeff;
    state->PC = 0x0000;

    //allocate 64k (65536 bytes)
    state->memory = (uint8_t*)malloc(sizeof(uint8_t) * 0xffff);

    state->cond.aux_carry = 0;
    state->cond.carry = 0;
    state->cond.parity = 0;
    state->cond.sign = 0;
    state->cond.zero = 0;

    return state;
}

void test(cpu* state) {
    state->A = 0xff;
    state->C = 0x3;

    state->memory[0x0000] = 0x81;

    execute(state);
}

// Output register contents to stdout
void dump_registers(cpu* state) {

    register_array = malloc(sizeof(cpu));

    uint8_t PC_low = state->PC;
    uint8_t PC_high = state->PC >> 8;

    uint8_t SP_low = state->SP;
    uint8_t SP_high = state->SP >> 8;

    uint8_t* register_pointer = &state->A;

    for(int i = 0; i < 7; i++) {
       register_array[i] = *(register_pointer + i);
    }

    register_array[7] = PC_high;
    register_array[8] = PC_low;
    register_array[9] = SP_high;
    register_array[10] = SP_low;

    for(int i = 0; i < 7; i++) {
        printf("%02x\n", register_array[i]);
    }

    for(int i = 7; i < 11; i+=2) {
        printf("%02x%02x\n", register_array[i], register_array[i+1]);
    }
}

// Check command line arguments

int check_args(char** argv) {
    if(strcmp(argv[1], "--dumpregisters") == 0) {
        return 0;
    }

    return -1;
}

void handle_args(int argc, char** argv, cpu* state) {

    if(argc >= 2) {
        if(check_args(argv) == 0) {
            dump_registers(state);
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

    display_intro();

    cpu* state = init_cpu();

    test(state);

    handle_args(argc, argv, state);

    return 0;
}
