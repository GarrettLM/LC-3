#include <stdio.h>
#include <stdint.h>
#include "memorymanagementunit.h"

#define HALT 0xF025
#define ADD 1
#define AND 5
#define BR 0
#define JMP 12
#define JSR 4
#define LD 2
#define LDI 10
#define LDR 6
#define LEA 14
#define NOT 9
#define RET 12
#define RTI 8
#define ST 3
#define STI 11
#define STR 7
#define TRAP 15
#define RESERVED 13

#define EVT_ADDRESS 0x0100
#define SSP 0x1000
#define PRIVELEGED_MODE_EXCEPTION 0
#define RESERVED_OPCODE_EXCEPTION 1

void initialize_machine();
uint16_t get_opcode(uint16_t instruction);
void branch(uint16_t instruction);
void add(uint16_t instruction);
void load(uint16_t instruction);
void store(uint16_t instruction);
void jump_subroutine(uint16_t instruction);
void and(uint16_t instruction);
void load_relative(uint16_t instruction);
void store_relative(uint16_t instruction);
void return_interrupt(uint16_t instruction);
void not(uint16_t instruction);
void load_indirect(uint16_t instruction);
void store_indirect(uint16_t instruction);
void jmp(uint16_t instruction);
void reserved_op(uint16_t instruction);
void load_effective_address(uint16_t instruction);
void trap(uint16_t instruction);
void set_cond_code(uint16_t);

typedef void (*opFunc)(uint16_t instruction);

opFunc inst_lut[16] = {
	branch,
	add,
	load,
	store,
	jump_subroutine,
	and,
	load_relative,
	store_relative,
	return_interrupt,
	not,
	load_indirect,
	store_indirect,
	jmp,
	reserved_op,
	load_effective_address,
	trap
};

int16_t r[8];
uint16_t pc = 0x3000;
uint16_t ir;	//Instruction Register
//Program Status Register
//Bits [2:0] are the condition codes.
//Bit 0 is set for P, bit 1 is set for Z, and bit 2 is set for N.
uint16_t psr;
int16_t a, b, c;	//registers used for executing instructions
uint16_t mar;	//Memory Address Register
uint16_t mdr; //Memory Data Register

int main(int argc, char *argv[]) {
	printf("Initializing machine\n");
	initialize_machine();
	printf("Machine initialized\n");
	FILE *program = fopen(argv[1], "rb");
	fread(&pc, 2, 1, program);
	printf("%04x\n", pc);
	load_file(pc, program);
	fclose(program);
	//return 0;

	while (get_mem(0xFFFE) & 0x8000) {
		//FETCH phase
		printf("%4x:\t", pc);
		mar = pc;
		ir = mdr = get_mem(mar);
		pc++;

		//DECODE phase
		//if (ir == HALT) return 0;
		uint16_t opcode = ir >> 12;
		inst_lut[opcode](ir);
		printf("Press enter to continue\n");
		getchar();
	}
}

void initialize_machine() {
	FILE *os = fopen("os.obj", "rb");
	fread(&pc, 2, 1, os);
	printf("%04x\n", pc);
	load_file(pc, os);
	fclose(os);
}

uint16_t getDr(uint16_t instruction) {
	return (instruction & 0x0E00) >> 9;
}

uint16_t getSrOne(uint16_t instruction) {
	return (instruction & 0x01C0) >> 6;
}

uint16_t getSrTwo(uint16_t instruction) {
	return instruction & 0x0007;
}

uint16_t get_opcode(uint16_t instruction) {
	return instruction >> 12;
}

//sign extend
uint16_t sext(uint16_t value, uint16_t size) {
	uint16_t mask = 1 << (size - 1);
	if (value & mask) {
		mask = 0xFFFF <<  size;
		return (value | mask);
	} else {
		return value;
	}
}

void branch(uint16_t instruction) {
	printf("Branch\n");
	//EVALUATE ADDRESS phase
	uint16_t address =  pc + sext(instruction & 0x01FF, 9);
	printf("%x\n", address);
	printf("%x\n", pc);
	printf("%x\n", sext(instruction & 0x01FF, 9));
	//FETCH OPERAND phase
	//EXECUTE phase
	//if any of the conditions are met
	if ((instruction >> 9) & psr) {
		printf("Condition to jump met\n");
		pc = address;
	} else {
		printf("Condition to jump not met\n");
	}
	//STORE RESULT phase
}

void add(uint16_t instruction) {
	//printf("Add\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t sr1 = getSrOne(instruction);
	uint16_t sr2 = 0;
	if (!(instruction & 0x0020)) sr2 = getSrTwo(instruction);
	//FETCH OPERAND phase
	a = r[sr1];
	if (sr2) b = r[sr2];
	else b = sext(instruction & 0x001F, 5);
	//EXECUTE phase
	c = a + b;
	set_cond_code(c);
	printf("%4x + %4x = %4x\n", a, b, c);
	//STORE RESULT phase
	r[dr] = c;
}

void load(uint16_t instruction) {
	printf("Load\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	mar = pc + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	mdr = get_mem(mar);
	set_cond_code(mdr);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = mdr;
}

void store(uint16_t instruction) {
	printf("Store\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	mar = pc + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	mdr = r[sr];
	//EXECUTE phase
	//STORE RESULT phase
	set_mem(mar, mdr);
}

void jump_subroutine(uint16_t instruction) {
	printf("jsr\n");
	r[7] = pc;
	if (instruction & 0x800) {
		pc += (instruction & 0x07FF);
	} else {
		uint16_t baseR = getSrOne(instruction);
		pc = r[baseR];
	}
}

void and(uint16_t instruction) {
	//printf("and\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t sr1 = getSrOne(instruction);
	uint16_t sr2 = 0;
	if (!(instruction & 0x0020)) sr2 = getSrTwo(instruction);
	//FETCH OPERAND phase
	a = r[sr1];
	if (sr2) b = r[sr2];
	else b = sext(instruction & 0x001F, 5);
	//EXECUTE phase
	c = a & b;
	set_cond_code(c);
	printf("%4x & %4x = %4x\n", a, b, c);
	//STORE RESULT phase
	r[dr] = c;
}

void load_relative(uint16_t instruction) {
	printf("ldr\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t base = r[getSrOne(instruction)];
	uint16_t offset = sext(instruction & 0x003F, 6);
	mar = base + offset;
	//FETCH OPERAND phase
	mdr = get_mem(mar);
	set_cond_code(mdr);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = mdr;
}

void store_relative(uint16_t instruction) {
	printf("str\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	uint16_t base = r[getSrOne(instruction)];
	uint16_t offset = sext(instruction & 0x003F, 6);
	mar = base + offset;
	//FETCH OPERAND phase
	mdr = r[sr];
	//EXECUTE phase
	//STORE RESULT phase
	set_mem(mar, mdr);
}

void return_interrupt(uint16_t instruction) {
	// Check if the CPU is in privileged mode
	if (psr & 0x8000) {
		// If PSR[15] = 0 throw a priveleged mode exception
		exception_routine(PRIVELEGED_MODE_EXCEPTION);
	} else {
		mar = r[6];
		pc = get_mem(mar);
		mar++;
		psr = get_mem(mar);
	}
}

void exception_routine(char exceptionVector) {
	mdr = psr;
	psr &= 0x7FFF;
	r[6] = SSP;
	mar = r[6];
	set_mem(mar, mdr);
	mar = r[6] + 1;
	mdr = pc;
	set_mem(mar, mdr);
	pc = get_mem(EVT_ADDRESS + exceptionVector);
}

void not(uint16_t instruction) {
	printf("not\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t sr = getSrOne(instruction);
	//FETCH OPERAND phase
	a = r[sr];
	//EXECUTE phase
	c = ~a;
	set_cond_code(c);
	//STORE RESULT phase
	r[dr] = c;
}

void load_indirect(uint16_t instruction) {
	printf("ldi\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	mar = pc + sext(instruction & 0x01FF, 9);
	printf("%x\n", mar);
	//FETCH OPERAND phase
	mdr = get_mem(mar);
	mar = mdr;
	printf("%x\n", mar);
	mdr = get_mem(mar);
	printf("%x\n", mdr);
	set_cond_code(mdr);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = mdr;
}

void store_indirect(uint16_t instruction) {
	printf("sti\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	mar = pc + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	mdr = get_mem(mar);
	mar = mdr;
	mdr = sr;
	//EXECUTE phase
	//STORE RESULT phase
	set_mem(mar, mdr);
}

//This doubles as the RET instruction
void jmp(uint16_t instruction) {
	printf("jmp");
	//EVALUATE ADDRESS phase
	short sr = getSrOne(instruction);
	//FETCH OPERAND phase
	//EXECUTE phase
	pc = r[sr];
	//STORE phase
}

void reserved_op(uint16_t instruction) {
		printf("Error! Use of reserved Opcode!\
		Address: %x\nInstruction: %x", (pc-1), instruction);
		exception_routine(RESERVED_OPCODE_EXCEPTION);
}

void load_effective_address(uint16_t instruction) {
	//printf("lea\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t immediateValue = pc + sext(instruction & 0x01FF, 9);
	printf("Loading address %4x to R%x\n", immediateValue, dr);
	//FETCH OPERAND phase
	set_cond_code(immediateValue);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = immediateValue;
}

void trap(uint16_t instruction) {
	//EVALUATE ADDRESS phase
	//FETCH OPERAND phase
	//EXECUTE phase
	mar = instruction & 0x00FF;
	mdr = get_mem(mar);
	r[7] = pc;
	pc = mdr;
	printf("Performing TRAP 0x%x\n", mar);
	//STORE RESULT phase
}

void set_cond_code(uint16_t result) {
	psr = psr & 0xFF00;
	if (result == 0) {
		printf("Condition code set to z\n");
		psr += 2;
	} else if (result & 0x8000) {
		//if the result was negative
		printf("Condition code set to n\n");
		psr += 4;
	} else {
		//if the result was positive
		printf("Condition code set to p\n");
		psr += 1;
	}
}
