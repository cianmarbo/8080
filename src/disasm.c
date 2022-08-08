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

