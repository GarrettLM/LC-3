#include <stdio.h>
#include "memorymanagementunit.h"
#include "consoleio.h"

static uint16_t memory[0xFFFF];

void load_file(uint16_t PC, FILE *fin) {
	for (int i = 0; !feof(fin); i++) {
		fread(&(memory[PC + i]), sizeof(uint16_t), 1, fin);
		memory[PC + i] = (memory[PC + i] << 8) | (memory[PC + i] >> 8);
		//printf("%04x\n", memory[PC + i]);
	}
	memory[0xFE04] = 1;
	memory[0xFFFE] = 0x8000;
}

uint16_t get_mem(uint16_t mar) {
	if (mar > 0xFDFF) {
		//since this will be an IO device access must be
		//synchronized
		//acquire lock for memory address
		if (mar == 0xFE00) return get_kbsr();
		else if (mar == 0xFe02) return get_kbdr();
		else if (mar == 0xFE04) return get_dsr();
		else return memory[mar];
		//release lock for memory address
	}
	return memory[mar];
}

void set_mem(uint16_t mar, uint16_t mdr) {
	if (mar > 0xFDFF) {
		//since this will be an IO device access must be
		//synchronized
		//acquire lock for memory address
		if (mar == 0xFE06) set_ddr(mdr);
		memory[mar] = mdr;
		//release lock for memory address
	}
	memory[mar] = mdr;
}
