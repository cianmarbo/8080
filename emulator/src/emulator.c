#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disasm.h"

const char* version_string = "0.0.3";
const char* build_date = __DATE__;

#define MAX_INTRO_LINES 14
#define MAX_INTRO_CHARS 50

#define PSW_FLAG        1

uint8_t* register_array = NULL;

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

static void ADD(cpu* state, uint8_t operand) {

    uint16_t result = (uint16_t)state->A + (uint16_t)operand;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (result > 0xff) ? 1 : 0;

    // assign result by doing bitwise AND on 0xff (255 base 10) to prevent overflow
    state->A = result & 0xff;
}

static void ADC(cpu* state, uint8_t operand) {

    uint16_t result = (uint16_t)state->A + (uint16_t)operand + state->cond.carry;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (result > 0xff) ? 1 : 0;

    state->A = result & 0xff;
}

static void SUB(cpu* state, uint8_t operand) {

    uint8_t result = state->A - operand;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    state->cond.carry = (state->A < operand) ? 1 : 0;

    state->A = result & 0xff;
}

static void SBB(cpu* state, uint8_t operand) {
    // if you're subbing two 8-bit values, result is never going to be 16-bits
    uint8_t result = state->A - operand - state->cond.carry;

    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;

    /*  if first value we're subtracting is less than the second value,
        in a multibyte subtraction this means we'll have to borrow, so we set the borrow flag.
        We can use SBB to subtract two values and the borrow flag (the carry, but its acting as a borrow)
    */
    state->cond.carry = (state->A < operand) ? 1 : 0;

    state->A = result & 0xff;
}

// Logical AND
static void ANA(cpu* state, uint8_t operand) {
    uint8_t result = state->A & operand;
    
    state->cond.carry = 0;
    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    // need to set parity
    state->A = result & 0xff;
}

// Logical exclusive OR (also known as Zero Accumulator)
static void XRA(cpu* state, uint8_t operand) {
    uint8_t result = state->A ^ operand;

    state->cond.carry = 0;
    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    //need to set parity and aux carry

    state->A = result & 0xff;
}

// Logical OR
static void ORA(cpu* state, uint8_t operand) {
    uint8_t result = state->A | operand;

    state->cond.carry = 0;
    state->cond.zero = (result & 0xff) ? 0 : 1;
    state->cond.sign = (result & 0x80) ? 1 : 0;
    //need to set parity

    state->A = result & 0xff;
}

static void CMP(cpu* state, uint8_t operand) {
    state->cond.zero = ((state->A - operand) & 0xff) ? 0 : 1;
    state->cond.carry = (state->A < operand) ? 1 : 0; // we use the carry as a borrow
    state->cond.sign = ((state->A - operand) & 0x80) ? 1 : 0;

    // need to implement parity, aux carry
}

static void PUSH(cpu* state, uint8_t reg1, uint8_t reg2, uint8_t push_psw) {
    //technically the stack is decremented after the operation but makes no difference here really
    state->SP -= 2;
    
    // create a PSW register from the conditions struct so we can perform a push
    if (push_psw) {
        uint8_t PSW = 
            (state->cond.sign << 7) |
            (state->cond.zero << 6) |
            (state->cond.aux_carry << 4) |
            (state->cond.parity << 2) |
            (1 << 1) |
            state->cond.carry;
        
        state->memory[state->SP] =  PSW;
    } else {
        state->memory[state->SP] = reg2;
    }

    state->memory[state->SP + 1] = reg1;
}

static void POP(cpu* state, uint8_t* reg1, uint8_t* reg2, uint8_t pop_psw) {
    /*

    The layout of the Program Status Word register (PSW) is as follows:

      7   6   5   4    3   2   1   0
    | S | Z | 0 | AC | 0 | P | 1 | C |
    
    The CPU flags are restored if PSW is specified
    Whatever byte is at the address SP is pointing too is popped into PSW (so here we just the set the conditions struct)
    */

    if (pop_psw) {
        state->cond.sign = ((state->memory[state->SP]) >> 7);
        state->cond.zero = ((state->memory[state->SP]) >> 6);
        state->cond.aux_carry = ((state->memory[state->SP]) >> 4);
        state->cond.parity = ((state->memory[state->SP]) >> 2);
        state->cond.carry = state->memory[state->SP];
    } else {
        *reg2 = state->memory[state->SP];
    }

    *reg1 = state->memory[state->SP+1];

    state->SP+=2;
}

static void MOV(uint8_t* op, uint8_t operand) {
    *(op) = operand;
}

void disassemble_instruction(uint8_t* instruction) {
    disassemble(instruction);
    printf("------------------------------\n");
}

void execute(cpu* state) {
    
    uint8_t* instruction = &state->memory[state->PC];
    uint16_t memory_offset;
    state->PC++; //increment the Program Counter after every instruction

    disassemble_instruction(instruction);

    switch(*instruction) {
        case 0x00:
            // state->PC += 2;
            break;
        case 0x01:
            break;
        //BEGIN MOV GROUP
        case 0x40:
            //MOV B, B
            MOV(&state->B, state->B);
            break;
        case 0x41:
            //MOV B, C
            MOV(&state->B, state->C);
            break;
        case 0x42:
            //MOV B, D
            MOV(&state->B, state->D);
            break;
        case 0x43:
            //MOV B, E
            MOV(&state->B, state->E);
            break;
        case 0x44:
            //MOV B, H
            MOV(&state->B, state->H);
            break;
        case 0x45:
            //MOV B, L
            MOV(&state->B, state->L);
            break;
        case 0x46:
            //MOV B, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->B, state->memory[memory_offset]);
            break;
        case 0x47:
            //MOV B, A
            MOV(&state->B, state->A);
            break;
        case 0x48:
            //MOV C, B
            MOV(&state->C, state->B);
            break;
        case 0x49:
            //MOV C, C
            MOV(&state->C, state->C);
            break;
        case 0x4A:
            //MOV C, D
            MOV(&state->C, state->D);
            break;
        case 0x4B:
            //MOV C, E
            MOV(&state->C, state->E);
            break;
        case 0x4C:
            //MOV C, H
            MOV(&state->C, state->H);
            break;
        case 0x4D:
            //MOV C, L
            MOV(&state->C, state->L);
            break;
        case 0x4E:
            //MOV C, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->C, state->memory[memory_offset]);
            break;
        case 0x4F:
            //MOV C, A
            MOV(&state->C, state->A);
            break;
        case 0x50:
            //MOV D, B
            MOV(&state->D, state->B);
            break;
        case 0x51:
            //MOV D, C
            MOV(&state->D, state->C);
            break;
        case 0x52:
            //MOV D, D
            MOV(&state->D, state->D);
            break;
        case 0x53:
            //MOV D, E
            MOV(&state->D, state->E);
            break;
        case 0x54:
            //MOV D, H
            MOV(&state->D, state->H);
            break;
        case 0x55:
            //MOV D, L
            MOV(&state->D, state->L);
            break;
        case 0x56:
            //MOV D, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->D, state->memory[memory_offset]);
            break;
        case 0x57:
            //MOV D, A
            MOV(&state->D, state->A);
            break;
        case 0x58:
            //MOV E, B
            MOV(&state->E, state->B);
            break;
        case 0x59:
            //MOV E, C
            MOV(&state->E, state->C);
            break;
        case 0x5A:
            //MOV E, D
            MOV(&state->E, state->D);
            break;
        case 0x5B:
            //MOV E, E
            MOV(&state->E, state->E);
            break;
        case 0x5C:
            //MOV E, H
            MOV(&state->E, state->H);
            break;
        case 0x5D:
            //MOV E, L
            MOV(&state->E, state->L);
            break;
        case 0x5E:
            //MOV E, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->E, state->memory[memory_offset]);
            break;
        case 0x5F:
            //MOV E, A
            MOV(&state->E, state->A);
            break;
        case 0x60:
            //MOV H, B
            MOV(&state->H, state->B);
            break;
        case 0x61:
            //MOV H, C
            MOV(&state->H, state->C);
            break;
        case 0x62:
            //MOV H, D
            MOV(&state->H, state->D);
            break;
        case 0x63:
            //MOV H, E
            MOV(&state->H, state->E);
            break;
        case 0x64:
            //MOV H, H
            MOV(&state->H, state->H);
            break;
        case 0x65:
            //MOV H, L
            MOV(&state->H, state->L);
            break;
        case 0x66:
            //MOV H, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->H, state->memory[memory_offset]);
            break;
        case 0x67:
            //MOV H, A
            MOV(&state->H, state->A);
            break;
        case 0x68:
            //MOV L, B
            MOV(&state->L, state->B);
            break;
        case 0x69:
            //MOV L, C
            MOV(&state->L, state->C);
            break;
        case 0x6A:
            //MOV L, D
            MOV(&state->L, state->D);
            break;
        case 0x6B:
            //MOV L, E
            MOV(&state->L, state->E);
            break;
        case 0x6C:
            //MOV L, H
            MOV(&state->L, state->H);
            break;
        case 0x6D:
            //MOV L, L
            MOV(&state->L, state->L);
            break;
        case 0x6E:
            //MOV L, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->L, state->memory[memory_offset]);
            break;
        case 0x6F:
            //MOV L, A
            MOV(&state->L, state->A);
            break;
        case 0x70:
            //MOV M, B
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->B);
            break;
        case 0x71:
            //MOV M, C
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->C);
            break;
        case 0x72:
            //MOV M, D
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->D);
            break;
        case 0x73:
            //MOV M, E
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->E);
            break;
        case 0x74:
            //MOV M, H
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->H);
            break;
        case 0x75:
            //MOV M, L
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->L);
            break;
        case 0x76:
            //MOV M, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->memory[memory_offset]);
            break;
        case 0x77:
            //MOV M, A
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->memory[memory_offset], state->A);
            break;
        case 0x78:
            //MOV A, B
            MOV(&state->A, state->B);
            break;
        case 0x79:
            //MOV A, C
            MOV(&state->A, state->C);
            break;
        case 0x7A:
            //MOV A, D
            MOV(&state->A, state->D);
            break;
        case 0x7B:
            //MOV A, E
            MOV(&state->A, state->E);
            break;
        case 0x7C:
            //MOV A, H
            MOV(&state->A, state->H);
            break;
        case 0x7D:
            //MOV A, L
            MOV(&state->A, state->L);
            break;
        case 0x7E:
            //MOV A, M
            memory_offset = (state->H << 8) | state->L;
            MOV(&state->A, state->memory[memory_offset]);
            break;
        case 0x7F:
            //MOV A, A
            MOV(&state->A, state->A);
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
            memory_offset = (state->H << 8) | state->L;
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
            memory_offset = (state->H << 8) | state->L;
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
        // BEGIN SUBTRACTION WITH BORROW GROUP
        case 0x96:
            //SUB M
            memory_offset = (state->H << 8) | state->L;
            SUB(state, state->memory[memory_offset]);
            break;
        case 0x97:
            //SUB A
            SUB(state, state->A);
            break;
        case 0x98:
            //SBB B
            SBB(state, state->B);
            break;
        case 0x99:
            //SBB C
            SBB(state, state->C);
            break;
        case 0x9A:
            //SBB D
            SBB(state, state->D);
            break;
        case 0x9B:
            //SBB E
            SBB(state, state->E);
            break;
        case 0x9C:
            // SBB H
            SBB(state, state->H);
            break;
        case 0x9D:
            // SBB L
            SBB(state, state->L);
            break;
        case 0x9E:
            // SBB M
            memory_offset = (state->H << 8) | state->L;
            SBB(state, state->memory[memory_offset]);
            break;
        case 0x9F:
            // SBB A
            SBB(state, state->A);
            break;
        // BEGIN LOGICAL AND GROUP
        case 0xA0:
            // ANA B
            ANA(state, state->B);
            break;
        case 0xA1:
            // ANA C
            ANA(state, state->C);
            break;
        case 0xA2:
            // ANA D
            ANA(state, state->D);
            break;
        case 0xA3:
            // ANA E
            ANA(state, state->E);
            break;
        case 0xA4:
            // ANA H
            ANA(state, state->H);
            break;
        case 0xA5:
            // ANA L
            ANA(state, state->L);
            break;
        case 0xA6:
            // ANA M
            memory_offset = (state->H << 8) | state->L;
            ANA(state, state->memory[memory_offset]);
            break;
        case 0xA7:
            // ANA A
            ANA(state, state->A);
            break;
        // BEGIN LOGICAL XOR GROUP
        case 0xA8:
            // XRA B
            XRA(state, state->B);
            break;
        case 0xA9:
            // XRA C
            XRA(state, state->C);
            break;
        case 0xAA:
            // XRA D
            XRA(state, state->D);
            break;
        case 0xAB:
            // XRA E
            XRA(state, state->E);
            break;
        case 0xAC:
            // XRA H
            XRA(state, state->H);
            break;
        case 0xAD:
            // XRA L
            XRA(state, state->L);
            break;
        case 0xAE:
            // XRA M
            memory_offset = (state->H << 8) | state->L;
            XRA(state, state->memory[memory_offset]);
            break;
        case 0xAF:
            // XRA A
            XRA(state, state->A);
            break;
        // BEGIN LOGICAL OR GROUP
        case 0xB0:
            // ORA B
            ORA(state, state->B);
            break;
        case 0xB1:
            // ORA C
            ORA(state, state->C);
            break;
        case 0xB2:
            // ORA D
            ORA(state, state->D);
            break;
        case 0xB3:
            // ORA E
            ORA(state, state->E);
            break;
        case 0xB4:
            // ORA H
            ORA(state, state->H);
            break;
        case 0xB5:
            // ORA L
            ORA(state, state->L);
            break;
        case 0xB6:
            // ORA M
            memory_offset = (state->H << 8) | state->L;
            ORA(state, state->memory[memory_offset]);
            break;
        case 0xB7:
            // ORA A
            ORA(state, state->A);
            break;
        // BEGIN COMPARISON GROUP
        case 0xB8:
            // CMP B
            CMP(state, state->B);
            break;
        case 0xB9:
            // CMP C
            CMP(state, state->C);
            break;
        case 0xBA:
            // CMP D
            CMP(state, state->D);
            break;
        case 0xBB:
            // CMP E
            CMP(state, state->E);
            break;
        case 0xBC:
            // CMP H
            CMP(state, state->H);
            break;
        case 0xBD:
            // CMP L
            CMP(state, state->L);
            break;
        case 0xBE:
            // CMP M
            memory_offset = (state->H << 8) | state->L;
            CMP(state, state->memory[memory_offset]);
            break;
        case 0xBF:
            // CMP A
            CMP(state, state->A);
            break;
        case 0xC1:
            // POP B and C ("POP B")
            POP(state, &state->B, &state->C, 0);
            break;
        case 0xD1:
            // POP D and E ("POP D") 
            POP(state, &state->D, &state->E, 0);
            break;
        case 0xE1:
            // POP H and L ("POP H") 
            POP(state, &state->H, &state->L, 0);
            break;
        case 0xF1:
            // POP A and PSW ("POP PSW")
            POP(state, &state->A, 0, PSW_FLAG);
            break;
        case 0xC5:
            // PUSH B and C ("PUSH B")
            PUSH(state, state->B, state->C, 0);
            break;
        case 0xD5:
            // PUSH D and E ("PUSH E")
            PUSH(state, state->D, state->E, 0);
            break;
        case 0xE5:
            // PUSH H and L ("PUSH H")
            PUSH(state, state->H, state->L, 0);
            break;
        case 0xF5:
            // PUSH A and PSW ("PUSH PSW")
            PUSH(state, state->A, 0, PSW_FLAG);
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
    // state->A = 0xff;
    state->B = 0x4;
    state->C = 0x3;
    // state->D = 0xf4;
    state->SP = 0x7FFF;

    state->memory[0x0000] = 0xc5; // push b and c
    state->memory[0x0001] = 0xe1; // pop into h and l


    // execute(state);

    // we update PC directly everytime we call execute()
    while(state->PC < 10) {
        execute(state);
    }
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

    display_intro();

    cpu* state = init_cpu();

    test(state);

    handle_args(argc, argv, state);

    return 0;
}
