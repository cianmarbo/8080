#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t disassemble(uint8_t* code_buffer, int pc) {

	uint8_t op_bytes = 1;
	uint8_t* code = code_buffer + pc;

	printf("%04x  ", pc);

	switch(*code) {
	case 0x00:
		printf("NOP\n");
		break;
	case 0x01:
		printf("LXI    B, %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x02:
		printf("STAX   B\n");
		break;
	case 0x03:
		printf("INX    B\n");
		break;
	case 0x04:
		printf("INR    B\n");
		break;
	case 0x05:
		printf("DCR    B\n");
		break;
	case 0x06:
		printf("MVI    B, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x07:
		printf("RLC\n");
		break;
	case 0x08:
		printf("NOP\n");
		break;
	case 0x09:
		printf("DAD    B\n");
		break;
	case 0xA:
		printf("LDAX   B\n");
		break;
	case 0xB:
		printf("DCX    B\n");
		break;
	case 0xC:
		printf("INR    C\n");
		break;
	case 0xD:
		printf("DCR    C\n");
		break;
	case 0xE:
		printf("MVI    C, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0xF:
		printf("RRC\n");
		break;
	case 0x10:
		printf("NOP\n");
		break;
	case 0x11:
		printf("LXI    D, %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x12:
		printf("STAX   D\n");
		break;
	case 0x13:
		printf("INX    D\n");
		break;
	case 0x14:
		printf("INR    D\n");
		break;
	case 0x15:
		printf("DCR    D\n");
		break;
	case 0x16:
		printf("MVI    D, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x17:
		printf("RAL\n");
		break;
	case 0x18:
		printf("NOP\n");
		break;
	case 0x19:
		printf("DAD    D\n");
		break;
	case 0x1A:
		printf("LDAX   D\n");
		break;
	case 0x1B:
		printf("DCX    D\n");
		break;
	case 0x1C:
		printf("INR    E\n");
		break;
	case 0x1D:
		printf("DCR    E\n");
		break;
	case 0x1E:
		printf("MVI    E, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x1F:
		printf("RAR\n");
		break;
	case 0x20:
		printf("NOP\n");
		break;
	case 0x21:
		printf("LXI    H, %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x22:
		printf("SHLD      %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x23:
		printf("INX    H\n");
		break;
	case 0x24:
		printf("INR    H\n");
		break;
	case 0x25:
		printf("DCR    H\n");
		break;
	case 0x26:
		printf("MVI    H, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x27:
		printf("DAA\n");
		break;
	case 0x28:
		printf("NOP\n");
		break;
	case 0x29:
		printf("DAD    H\n");
		break;
	case 0x2A:
		printf("LHLD   %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x2B:
		printf("DCX    H\n");
		break;
	case 0x2C:
		printf("INR    L\n");
		break;
	case 0x2D:
		printf("DCR    L\n");
		break;
	case 0x2E:
		printf("MVI    L, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x2F:
		printf("CMA\n");
		break;
	case 0x30:
		printf("NOP\n");
		break;
	case 0x31:
		printf("LXI    SP, %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x32:
		printf("STA    %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x33:
		printf("INX    SP\n");
		break;
	case 0x34:
		printf("INR    M\n");
		break;
	case 0x35:
		printf("DCR    M\n");
		break;
	case 0x36:
		printf("MVI    M, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x37:
		printf("STC\n");
		break;
	case 0x38:
		printf("NOP\n");
		break;
	case 0x39:
		printf("DAD   SP\n");
		break;
	case 0x3A:
		printf("LDA   %02x%02x\n", code[2], code[1]);
		op_bytes = 3;
		break;
	case 0x3B:
		printf("DCX   SP\n");
		break;
	case 0x3C:
		printf("INR   A\n");
		break;
	case 0x3D:
		printf("DCR   A\n");
		break;
	case 0x3E:
		printf("MVI   A, %02x\n", code[1]);
		op_bytes = 2;
		break;
	case 0x3F:
		printf("CMC\n");
		break;
	case 0x40:
		printf("MOV   B, B\n");
		break;
	case 0x41:
		printf("MOV   B, C\n");
		break;
	case 0x42:
		printf("MOV   B, D\n");
		break;
	case 0x43:
		printf("MOV   B, E\n");
		break;
	case 0x44:
		printf("MOV   B, H\n");
		break;
	case 0x45:
		printf("MOV   B, L\n");
		break;
	case 0x46:
		printf("MOV   B, M\n");
		break;
	case 0x47:
		printf("MOV   B, A\n");
		break;
	case 0x48:
		printf("MOV   C, B\n");
		break;
	case 0x49:
		printf("MOV   C, C\n");
		break;
	case 0x4A:
		printf("MOV   C, D\n");
		break;
	case 0x4B:
		printf("MOV   C, E\n");
		break;
	case 0x4C:
		printf("MOV   C, H\n");
		break;
	case 0x4D:
		printf("MOV   C, L\n");
		break;
	case 0x4E:
		printf("MOV   C, M\n");
		break;
	case 0x4F:
		printf("MOV   C, A\n");
		break;
	case 0x50:
		printf("MOV   D, B\n");
		break;
	case 0x51:
		printf("MOV   D, C\n");
		break;
	case 0x52:
		printf("MOV   D, D\n");
		break;
	case 0x53:
		printf("MOV   D, E\n");
		break;
	case 0x54:
		printf("MOV   D, H\n");
		break;
	case 0x55:
		printf("MOV   D, L\n");
		break;
	case 0x56:
		printf("MOV   D, M\n");
		break;
	case 0x57:
		printf("MOV   D, A\n");
		break;
	case 0x58:
		printf("MOV   E, B\n");
		break;
	case 0x59:
		printf("MOV   E, C\n");
		break;
	case 0x5A:
		printf("MOV   E, D\n");
		break;
	case 0x5B:
		printf("MOV   E, E\n");
		break;
	case 0x5C:
		printf("MOV   E, H\n");
		break;
	case 0x5D:
		printf("MOV   E, L\n");
		break;
	case 0x5E:
		printf("MOV   E, M\n");
		break;
	case 0x5F:
		printf("MOV   E, A\n");
		break;
	case 0x60:
		printf("MOV   H, B\n");
		break;
	case 0x61:
		printf("MOV   H, C\n");
		break;
	case 0x62:
		printf("MOV   H, D\n");
		break;
	case 0x63:
		printf("MOV   H, E\n");
		break;
	case 0x64:
		printf("MOV   H, H\n");
		break;
	case 0x65:
		printf("MOV   H, L\n");
		break;
	case 0x66:
		printf("MOV   H, M\n");
		break;
	case 0x67:
		printf("MOV   H, A\n");
		break;
	case 0x68:
		printf("MOV   L, B\n");
		break;
	case 0x69:
		printf("MOV   L, C\n");
		break;
	case 0x6A:
		printf("MOV   L, D\n");
		break;
	case 0x6B:
		printf("MOV   L, E\n");
		break;
	case 0x6C:
		printf("MOV   L, H\n");
		break;
	case 0x6D:
		printf("MOV   L, L\n");
		break;
	case 0x6E:
		printf("MOV   L, M\n");
		break;
	case 0x6F:
		printf("MOV   L, A\n");
		break;
	case 0x70:
		printf("MOV   M, B\n");
		break;
	case 0x71:
		printf("MOV   M, C\n");
		break;
	case 0x72:
		printf("MOV   M, D\n");
		break;
	case 0x73:
		printf("MOV   M, E\n");
		break;
	case 0x74:
		printf("MOV   M, H\n");
		break;
	case 0x75:
		printf("MOV   M, L\n");
		break;
	case 0x76:
		printf("MOV   M, M\n");
		break;
	case 0x77:
		printf("MOV   M, A\n");
		break;
	case 0x78:
		printf("MOV   A, B\n");
		break;
	case 0x79:
		printf("MOV   A, C\n");
		break;
	case 0x7A:
		printf("MOV   A, D\n");
		break;
	case 0x7B:
		printf("MOV   A, E\n");
		break;
	case 0x7C:
		printf("MOV   A, H\n");
		break;
	case 0x7D:
		printf("MOV   A, L\n");
		break;
	case 0x7E:
		printf("MOV   A, M\n");
		break;
	case 0x7F:
		printf("MOV   A, A\n");
		break;
	case 0x80:
		printf("ADD   B\n");
		break;
	case 0x81:
		printf("ADD   C\n");
		break;
	case 0x82:
		printf("ADD   D\n");
		break;
	case 0x83:
		printf("ADD   E\n");
		break;
	case 0x84:
		printf("ADD   H\n");
		break;
	case 0x85:
		printf("ADD   L\n");
		break;
	case 0x86:
		printf("ADD   M\n");
		break;
	case 0x87:
		printf("ADD   A\n");
		break;
	case 0x8A:
		printf("ADC   B\n");
		break;
	case 0x8B:
		printf("ADC   C\n");
		break;
	case 0x8C:
		printf("ADC   D\n");
		break;
	case 0x8D:
		printf("ADC   E\n");
		break;
	case 0x8E:
		printf("ADC   H\n");
		break;
	case 0x8F:
		printf("ADC   A\n");
		break;
	case 0x90:
		printf("SUB   B\n");
		break;
	case 0x91:
		printf("SUB   C\n");
		break;
	case 0x92:
		printf("SUB   D\n");
		break;
	case 0x93:
		printf("SUB   E\n");
		break;
	case 0x94:
		printf("SUB   H\n");
		break;
	case 0x95:
		printf("SUB   L\n");
		break;
	case 0x96:
		printf("SUB   M\n");
		break;
	case 0x97:
		printf("SUB   A\n");
		break;
	case 0x98:
		printf("SBB   B\n");
		break;
	case 0x99:
		printf("SBB   C\n");
		break;
	case 0x9A:
		printf("SBB   D\n");
		break;
	case 0x9B:
		printf("SBB   E\n");
		break;
	case 0x9C:
		printf("SBB   H\n");
		break;
	case 0x9D:
		printf("SBB   L\n");
		break;
	case 0x9E:
		printf("SBB   M\n");
		break;
	case 0x9F:
		printf("SBB   A\n");
		break;
	case 0xA0:
		printf("ANA   B\n");
		break;
	case 0xA1:
		printf("ANA   C\n");
		break;
	case 0xA2:
		printf("ANA   D\n");
		break;
	case 0xA3:
		printf("ANA   E\n");
		break;
	case 0xA4:
		printf("ANA   H\n");
		break;
	case 0xA5:
		printf("ANA   L\n");
		break;
	case 0xA6:
		printf("ANA   M\n");
		break;
	case 0xA7:
		printf("ANA   A\n");
		break;
	case 0xA8:
		printf("XRA   B\n");
		break;
	case 0xA9:
		printf("XRA   C\n");
		break;
	case 0xAA:
		printf("XRA   D\n");
		break;
	case 0xAB:
		printf("XRA   E\n");
		break;
	case 0xAC:
		printf("XRA   H\n");
		break;
	case 0xAD:
		printf("XRA   L\n");
		break;
	case 0xAE:
		printf("XRA   M\n");
		break;
	case 0xAF:
		printf("XRA   A\n");
		break;
	default:
		printf("Unhandled Instruction....\n");
		break;
	}

	return op_bytes;

} 

int main(int argc, char** argv) {

	char* array[7];

	/*

	array[0] = " ________  ________  ________  ________      \n";
	array[1] = "|\   __  \|\   __  \|\   __  \|\   __  \     \n";
	array[2] = "\ \  \|\  \ \  \|\  \ \  \|\  \ \  \|\  \    \n";
	array[3] = " \ \   __  \ \  \\\  \ \   __  \ \  \\\  \   \n";
	array[4] = "  \ \  \|\  \ \  \\\  \ \  \|\  \ \  \\\  \  \n";
	array[5] = "   \ \_______\ \_______\ \_______\ \_______\ \n";
	array[6] = "    \|_______|\|_______|\|_______|\|_______| \n";

	*/

	array[0] = " ________  ________  ________  ________                       \n";
	array[1] = "|\\   __  \\|\\   __  \\|\\   __  \\|\\   __  \\              \n";
	array[2] = "\\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\    \n";
	array[3] = " \\ \\   __  \\ \\  \\|\\  \\ \\   __  \\ \\  \\|\\  \\         \n";
	array[4] = "  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\|\\  \\    \n";
	array[5] = "   \\ \\_______\\ \\_______\\ \\_______\\ \\_______\\         \n";
	array[6] = "    \\|_______|\\|_______|\\|_______|\\|_______|              \n";

	

	for(int i = 0; i < 7; i++) {
		printf("%s", array[i]);
	}

	printf("\nwelcome to eighty eighty, an Intel 8080 disassembler!\n");
	printf("------------by cianmarbo\n");
	printf("\nmake sure your rom is valid ;)\n\n");

	FILE* rom_file = fopen(argv[1], "r");

	if(!rom_file) {
		printf("please point to a valid ROM...\n");
		exit(0);
	} 

	fseek(rom_file, 0, SEEK_END);
	int file_size = ftell(rom_file);
	rewind(rom_file);

	printf("\nFile size......%d\n", file_size);

	uint8_t* rom_buffer = (uint8_t*) malloc(file_size);

	fread(rom_buffer, file_size, 1, rom_file);
	fclose(rom_file);

	unsigned int pc = 0;

	while(pc < file_size) {

		pc += disassemble(rom_buffer, pc);
	}

	free(rom_buffer);

	return 0;
}

