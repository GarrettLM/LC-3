#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "parser.h"
#include "tokenizer.h"
#include "symboltable.h"

static char *token;

// Used for parsing the DR, SR1, and SR2/imm5
uint16_t parse_operand(char *buffer, short operandType) {
	uint16_t operand = 0;
	if (buffer[0] == 'R' || buffer[0] == 'r') {
		operand = string_to_uint16(buffer+1, '#');
		if (operand > 7 || operand < 0) {
			//printf("Error: An incorrect number is used for a register on line %d!\nAbort!\n", line_number);
			return -1;
		}
	} else if (buffer[0] == '#' && operandType == OPERAND_TWO) {
		operand = string_to_uint16(buffer+1, '#');
		if (operand > 15 || operand < -16) {
			//printf("Error: An incorrect number is used for a register on line %d!\nAbort!\n", line_number);
			return -1;
		}
		if (operand < 0) operand = operand & 0x001F;
		operand = operand | 0x20;	//Set bit 5 of the instruction
	} else if ((buffer[0] == 'x' || buffer[0] == 'X') && operandType == OPERAND_TWO) {
		operand = string_to_uint16(buffer+1, 'x');
		if (operand > 15 || operand < -16) {
			//printf("Error: An incorrect number is used for a register on line %d!\nAbort!\n", line_number);
			return -1;
		}
		if (operand < 0) operand = operand & 0x001F;
		operand = operand | 0x20;	//Set bit 5 of the instruction
	} else if (buffer[0] >= '0' && buffer[0] <= '9' && operandType == OPERAND_TWO) {
		operand = string_to_uint16(buffer, '#');
		if (operand > 15 || operand < -16) {
			//printf("Error: An incorrect number is used for a register on line %d!\nAbort!\n", line_number);
			return -1;
		}
		if (operand < 0) operand = operand & 0x001F;
		operand = operand | 0x20;	//Set bit 5 of the instruction
	} else {
		switch (operandType) {
			case DR:
				//printf("Error: Destination Register on line %d not formatted properly!\nAbort!\n", line_number);
				break;
			case SRONE:
				//printf("Error: SR1 on line %d not formatted properly!\nAbort!\n", line_number);
				break;
			case OPERAND_TWO:
				//printf("Error: Operand Two on line %d not formatted properly!\nAbort!\n", line_number);
				break;
		}
		return -1;
	}

	if (operandType == DR) operand = operand << 9;
	else if (operandType == SRONE) operand = operand << 6;
	return operand;
}

uint16_t parse_pc_offset(char *buffer, uint16_t pc, short offsetLength) {
	uint16_t offset = 0;
	if (buffer[0] == '#') {
		offset = string_to_uint16(buffer+1, '#');
	} else if (buffer[0] == 'x' || buffer[0] == 'X') {
		offset = string_to_uint16(buffer+1, 'x');
	} else {
		ste *symentry = search_symtable(buffer);
		if (symentry == NULL) {
			printf("Error: Could not find label %s in symbol table!\nAbort!\n", buffer);
			return -1;
		}
		offset = symentry->address;
		offset -= pc;
	}
	if (offsetLength == 9 && (offset > 255 || offset < -256)) {
	} else if (offsetLength == 11 && (offset > 1023 || offset < -1024)) {
	}

	// Eliminate the leading 1's if the number is negative
	if (offsetLength == 9) offset = offset & 0x01FF;
	else if (offsetLength == 11) offset = offset & 0x07FF;
	return offset;
}

uint16_t parse_r_offset(char *buffer) {
	uint16_t offset = 0;
	if (buffer[0] == '#') {
		offset = string_to_uint16(buffer+1, '#');
	} else if (buffer[0] == 'x' || buffer[0] == 'X') {
		offset = string_to_uint16(buffer+1, 'x');
	}

	if (offset < 31 || offset > -32) {
		printf("Error: offset outside of the range of values!\nAbort!\n");
		return -1;
	}

	offset = offset & 0x003F;

	return offset;
}

uint16_t string_to_uint16(char *str, char mode) {
	uint16_t result = 0;
	if (mode == '#') {
		short i = 0;
		if (str[0] == '-') i = 1;
		while (str[i]) {
			result *= 10;
			if (str[i] >= '0' && str[i] <= '9') {
				result += str[i] - 48;
			} else {}
			i++;
		}
		if (str[0] == '-') result *= -1;
	} else if (mode == 'x') {
		for (short i = 0; str[i]; i++) {
			result = result << 4;
			if (str[i] >= '0' && str[i] <= '9') {
				result += str[i] - 48;
			} else if (str[i] >= 'a' && str[i] <= 'f') {
				result += str[i] - 87;
			} else if (str[i] >= 'A' && str[i] <= 'F') {
				result += str[i] - 55;
			} else {}
		}
	}
	return result;
}

int parse_add(uint16_t *instruction, uint16_t lc) {
	//Parse the destination register
	next_token(&token);
	uint16_t dr = parse_operand(token, DR);
	if (dr == -1) return 1;
	*instruction = *instruction | dr;

	//Parse the first operand (source register one)
	next_token(&token);
	uint16_t operandOne = parse_operand(token, SRONE);
	if (operandOne == -1) return 1;
	*instruction = *instruction | operandOne;

	//Parse the second operand
	next_token(&token);
	uint16_t operandTwo = parse_operand(token, OPERAND_TWO);
	if (operandTwo == -1) return 1;
	*instruction = *instruction | operandTwo;
	return 0;
}

int parse_br(uint16_t *instruction, uint16_t lc) {
	//Find next token
	next_token(&token);
	uint16_t offset = parse_pc_offset(token, lc+1, 9);
	if (offset == -1) return 1;
	*instruction = *instruction | offset;
	return 0;
}

int parse_jmp(uint16_t *instruction, uint16_t lc) {
	//Parse the base register
	next_token(&token);
	uint16_t baseRegister = parse_operand(token, SRONE);
	if (baseRegister == -1) return 1;
	*instruction = *instruction | baseRegister;
	return 0;
}

int parse_jsr(uint16_t *instruction, uint16_t lc) {
	//Parse the offset
	next_token(&token);
	uint16_t offset = parse_pc_offset(token, lc+1, 11);
	*instruction = *instruction | offset;
	return 0;
}

int parse_ld(uint16_t *instruction, uint16_t lc) {
	//Parse the destination register
	next_token(&token);
	uint16_t dr = parse_operand(token, DR);
	if (dr == -1) return 1;
	*instruction = *instruction | dr;

	//Parse the offset
	next_token(&token);
	uint16_t offset = parse_pc_offset(token, lc+1, 9);
	*instruction = *instruction | offset;
	return 0;
}

int parse_ldr(uint16_t *instruction, uint16_t lc) {
	//Parse the destination register
	next_token(&token);
	uint16_t dr = parse_operand(token, DR);
	if (dr == -1) return 1;
	*instruction = *instruction | dr;

	//Parse the base register
	next_token(&token);
	uint16_t baseRegister = parse_operand(token, SRONE);
	if (baseRegister == -1) return 1;
	*instruction = *instruction | baseRegister;

	//Parse the offset
	next_token(&token);
	uint16_t offset = string_to_uint16(token+1, token[0]);
	if (offset > 31 || offset < -32) {
		//printf("Error: Offset out of range on line %d!\nAbort!\n",line_number);
		return 1;
	}
	*instruction = *instruction | offset;
	return 0;
}

int parse_not(uint16_t* instruction, uint16_t lc) {
	//Parse the destination register
	next_token(&token);
	uint16_t dr = parse_operand(token, DR);
	if (dr == -1) return 1;
	*instruction = *instruction | dr;

	//Parse the operand (source register)
	next_token(&token);
	uint16_t operand = parse_operand(token, SRONE);
	if (operand == -1) return 1;
	*instruction = *instruction | operand;
	return 0;
}

int parse_trap(uint16_t *instruction, uint16_t lc) {
	//Parse TRAP Vector
	next_token(&token);
	uint16_t trapVector = 0;
	if (token[0] == '#') trapVector = string_to_uint16(token+1, '#');
	else if (token[0] == 'x' || token[0] == 'X') trapVector = string_to_uint16(token+1, 'x');
	if (trapVector > 0xFF) return 1;
	*instruction = *instruction | trapVector;
	return 0;
}
