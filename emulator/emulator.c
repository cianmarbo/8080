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
        //BEGIN ADDITION GROUP
        case 0x80:
            //ADD B
            result = (uint16_t)state->A + (uint16_t)state->B;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            // assign result by doing bitwise AND on 0xff (255 base 10) to prevent overflow
            state->A = result & 0xff;
            break;
        case 0x81:
            // ADD C
            result = (uint16_t)state->A + (uint16_t)state->C;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x82:
            //ADD D
            result = (uint16_t)state->A + (uint16_t)state->D;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x83:
            //ADD E
            result = (uint16_t)state->A + (uint16_t)state->E;
            
            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x84:
            //ADD H
            result = (uint16_t)state->A + (uint16_t)state->H;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x85:
            //ADD L
            result = (uint16_t)state->A + (uint16_t)state->L;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x86:
            //ADD M (memory address referenced by combo of H and L)
            memory_offset = (state->H << 8) + state->L;
            result = (uint16_t)state->A + (uint16_t)state->memory[memory_offset];

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x87:
            // ADD A
            result = (uint16_t)state->A + (uint16_t)state->A;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x88:
            //ADC B
            result = (uint16_t)state->A + (uint16_t)state->B + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x89:
            //ADC C
            result = (uint16_t)state->A + (uint16_t)state->C + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8A:
            //ADC D
            result = (uint16_t)state->A + (uint16_t)state->D + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8B:
            //ADC E
            result = (uint16_t)state->A + (uint16_t)state->E + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8C:
            //ADC H
            result = (uint16_t)state->A + (uint16_t)state->H + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8D:
            //ADC L
            result = (uint16_t)state->A + (uint16_t)state->L + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8E:
            //ADC M (memory address made up of H + L combo)
            memory_offset = (state->H << 8) + state->L;
            result = (uint16_t)state->A + (uint16_t)state->memory[memory_offset] + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        case 0x8F:
            //ADC A
            result = (uint16_t)state->A + (uint16_t)state->A + state->cond.carry;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 1 : 0;

            state->A = result & 0xff;
            break;
        //BEGIN SUBTRACTION GROUP
        case 0x90:
            //SUB B
            result = (uint16_t)state->A - (uint16_t)state->B;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x91:
            //SUB C
            result = (uint16_t)state->A - (uint16_t)state->C;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x92:
            //SUB D
            result = (uint16_t)state->A - (uint16_t)state->D;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x93:
            //SUB E
            result = (uint16_t)state->A - (uint16_t)state->E;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x94:
            //SUB H
            result = (uint16_t)state->A - (uint16_t)state->H;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x95:
            //SUB L
            result = (uint16_t)state->A - (uint16_t)state->L;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x96:
            //SUB M
            memory_offset = (state->H << 8) + state->L;
            result = (uint16_t)state->A - (uint16_t)state->memory[memory_offset];

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
            break;
        case 0x97:
            //SUB A
            result = (uint16_t)state->A - (uint16_t)state->A;

            state->cond.zero = (result & 0xff) ? 0 : 1;
            state->cond.sign = (result & 0x80) ? 1 : 0;
            state->cond.carry = (result > 0xff) ? 0 : 1;

            state->A = result & 0xff;
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
    state->L = 0x10;

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

    uint8_t PC_high = state->PC;
    uint8_t PC_low = state->PC >> 8;

    uint8_t SP_high = state->SP;
    uint8_t SP_low = state->SP >> 8;

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
        printf("%02x%02x\n", register_array[i+1], register_array[i]);
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

    if(argc < 2) {
        printf("usage: [romfile]\n");
    }

    uint8_t* rom_buffer = open_rom(argv[1]);

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

int main(int argc, char** argv) {

    display_intro();

    cpu* state = init_cpu();

    test(state);

    handle_args(argc, argv, state);

    return 0;
}
