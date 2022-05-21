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

#define EVT_ADDRESS x0100
#define PRIVELEGED_MODE_EXCEPTION 0
#define RESERVED_OPCODE_EXCPETION 1

void initializeMachine();
uint16_t getOpcode(uint16_t instruction);
void branch(uint16_t instruction);
void add(uint16_t instruction);
void load(uint16_t instruction);
void store(uint16_t instruction);
void jumpSubroutine(uint16_t instruction);
void and(uint16_t instruction);
void loadRelative(uint16_t instruction);
void storeRelative(uint16_t instruction);
void returnInterrupt(uint16_t instruction);
void not(uint16_t instruction);
void loadIndirect(uint16_t instruction);
void storeIndirect(uint16_t instruction);
void jmp(uint16_t instruction);
void reservedOp(uint16_t instruction);
void loadEffectiveAddress(uint16_t instruction);
void trap(uint16_t instruction);
void setCondCode(uint16_t);

typedef void (*opFunc)(uint16_t instruction);

opFunc inst_lut[16] = {
	branch,
	add,
	load,
	store,
	jumpSubroutine,
	and,
	loadRelative,
	storeRelative,
	returnInterrupt,
	not,
	loadIndirect,
	storeIndirect,
	jmp,
	reservedOp,
	loadEffectiveAddress,
	trap
};

int16_t r[8];
uint16_t PC = 0x3000;
uint16_t IR;	//Instruction Register
//Program Status Register
//Bits [2:0] are the condition codes.
//Bit 0 is set for P, bit 1 is set for Z, and bit 2 is set for N.
uint16_t psr;
int16_t a, b, c;	//registers used for executing instructions
uint16_t MAR;	//Memory Address Register
uint16_t MDR; //Memory Data Register

int main(int argc, char *argv[]) {
	printf("Initializing machine\n");
	initializeMachine();
	printf("Machine initialized\n");
	FILE *program = fopen(argv[1], "rb");
	fread(&PC, 2, 1, program);
	printf("%04x\n", PC);
	loadFile(PC, program);
	fclose(program);
	//return 0;

	while (getMA(0xFFFE) & 0x8000) {
		//FETCH phase
		printf("%4x:\t", PC);
		MAR = PC;
		IR = MDR = getMA(MAR);
		PC++;

		//DECODE phase
		//if (IR == HALT) return 0;
		uint16_t opcode = IR >> 12;
		inst_lut[opcode](IR);
		printf("Press enter to continue\n");
		getchar();
	}
}

void initializeMachine() {
	FILE *os = fopen("os.obj", "rb");
	fread(&PC, 2, 1, os);
	printf("%04x\n", PC);
	loadFile(PC, os);
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

uint16_t getOpcode(uint16_t instruction) {
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
	uint16_t address =  PC + sext(instruction & 0x01FF, 9);
	printf("%x\n", address);
	printf("%x\n", PC);
	printf("%x\n", sext(instruction & 0x01FF, 9));
	//FETCH OPERAND phase
	//EXECUTE phase
	//if any of the conditions are met
	if ((instruction >> 9) & psr) {
		printf("Condition to jump met\n");
		PC = address;
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
	setCondCode(c);
	printf("%4x + %4x = %4x\n", a, b, c);
	//STORE RESULT phase
	r[dr] = c;
}

void load(uint16_t instruction) {
	printf("Load\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	MAR = PC + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	MDR = getMA(MAR);
	setCondCode(MDR);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = MDR;
}

void store(uint16_t instruction) {
	printf("Store\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	MAR = PC + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	MDR = r[sr];
	//EXECUTE phase
	//STORE RESULT phase
	setMA(MAR, MDR);
}

void jumpSubroutine(uint16_t instruction) {
	printf("jsr\n");
	r[7] = PC;
	if (instruction & 0x800) {
		PC += (instruction & 0x07FF);
	} else {
		uint16_t baseR = getSrOne(instruction);
		PC = r[baseR];
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
	setCondCode(c);
	printf("%4x & %4x = %4x\n", a, b, c);
	//STORE RESULT phase
	r[dr] = c;
}

void loadRelative(uint16_t instruction) {
	printf("ldr\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t base = r[getSrOne(instruction)];
	uint16_t offset = sext(instruction & 0x003F, 6);
	MAR = base + offset;
	//FETCH OPERAND phase
	MDR = getMA(MAR);
	setCondCode(MDR);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = MDR;
}

void storeRelative(uint16_t instruction) {
	printf("str\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	uint16_t base = r[getSrOne(instruction)];
	uint16_t offset = sext(instruction & 0x003F, 6);
	MAR = base + offset;
	//FETCH OPERAND phase
	MDR = r[sr];
	//EXECUTE phase
	//STORE RESULT phase
	setMA(MAR, MDR);
}

void returnInterrupt(uint16_t instruction) {
	// Check if the CPU is in privileged mode
	if (psr & x8000) {
		// If PSR[15] = 0 throw a priveleged mode exception
		exceptionRoutine(PRIVELEGED_MODE_EXCEPTION);
	} else {
		MAR = r[6];
		PC = getMA(MAR);
		MAR++;
		psr = getMA(MAR);
	}
}

void exceptionRoutine(char exceptionVector) {
	MDR = psr;
	psr &= x7FFF;
	r[6] = SSP;
	MAR = r[6];
	setMA(MAR, MDR);
	MAR = r[6] + 1;
	MDR = PC;
	setMA(r[MAR, MDR);
	PC = getMA(EVT_ADDRESS + exceptionVector);
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
	setCondCode(c);
	//STORE RESULT phase
	r[dr] = c;
}

void loadIndirect(uint16_t instruction) {
	printf("ldi\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	MAR = PC + sext(instruction & 0x01FF, 9);
	printf("%x\n", MAR);
	//FETCH OPERAND phase
	MDR = getMA(MAR);
	MAR = MDR;
	printf("%x\n", MAR);
	MDR = getMA(MAR);
	printf("%x\n", MDR);
	setCondCode(MDR);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = MDR;
}

void storeIndirect(uint16_t instruction) {
	printf("sti\n");
	//EVALUATE ADDRESS phase
	uint16_t sr = getDr(instruction);
	MAR = PC + sext(instruction & 0x01FF, 9);
	//FETCH OPERAND phase
	MDR = getMA(MAR);
	MAR = MDR;
	MDR = sr;
	//EXECUTE phase
	//STORE RESULT phase
	setMA(MAR, MDR);
}

//This doubles as the RET instruction
void jmp(uint16_t instruction) {
	printf("jmp");
	//EVALUATE ADDRESS phase
	short sr = getSrOne(instruction);
	//FETCH OPERAND phase
	//EXECUTE phase
	PC = r[sr];
	//STORE phase
}

void reservedOp(uint16_t instruction) {
		printf("Error! Use of reserved Opcode!\
		Address: %x\nInstruction: %x", (PC-1), instruction);
		exceptionRoutine(RESERVED_OPCODE_EXCEPTION);
}

void loadEffectiveAddress(uint16_t instruction) {
	//printf("lea\n");
	//EVALUATE ADDRESS phase
	uint16_t dr = getDr(instruction);
	uint16_t immediateValue = PC + sext(instruction & 0x01FF, 9);
	printf("Loading address %4x to R%x\n", immediateValue, dr);
	//FETCH OPERAND phase
	setCondCode(immediateValue);
	//EXECUTE phase
	//STORE RESULT phase
	r[dr] = immediateValue;
}

void trap(uint16_t instruction) {
	//EVALUATE ADDRESS phase
	//FETCH OPERAND phase
	//EXECUTE phase
	MAR = instruction & 0x00FF;
	MDR = getMA(MAR);
	r[7] = PC;
	PC = MDR;
	printf("Performing TRAP 0x%x\n", MAR);
	//STORE RESULT phase
}

void setCondCode(uint16_t result) {
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
