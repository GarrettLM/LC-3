#include <stdio.h>
#include "memorymanagementunit.h"

static uint16_t memory[0xFFFF];

void load_file(uint16_t PC, FILE *fin) {
	for (int i = 0; !feof(fin); i++) {
		fread(&(memory[PC + i]), sizeof(uint16_t), 1, fin);
		//memory[PC + i] = (memory[PC + i] << 8) | (memory[PC + i] >> 8);
		printf("%04x\n", memory[PC + i]);
	}
	memory[0xFE04] = 1;
	memory[0xFFFE] = 0x8000;
}

uint16_t get_mem(uint16_t mar) {
	if (mar > 0xFDFF) {
		//since this will be an IO device access must be
		//synchronized
		uint16_t mdr;
		//acquire lock for memory address
		mdr = memory[mar];
		//release lock for memory address
		return mdr;
	}
	return memory[mar];
}

void set_mem(uint16_t mar, uint16_t mdr) {
	if (mar > 0xFDFF) {
		//since this will be an IO device access must be
		//synchronized
		//acquire lock for memory address
		memory[mar] = mdr;
		//release lock for memory address
	}
	memory[mar] = mdr;
}
