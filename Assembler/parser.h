#ifndef PARSER_FILE
#define PARSER_FILE

//These constants are passed to the parse_operand function
//to indicate the operand type to parse
#define DR 0	//Parse the destination register (this is also used for the source register for the load instruction)
#define SRONE 1	//Parse the first source register for an instruction
#define OPERAND_TWO 2	//Parse the second operand for an instruction (this could be a register or an immediate value)

//Buffer is a string representation of a register, address, or immediate value to be parsed
uint16_t parse_operand(char *buffer, short operandType);
//This is used to parse PC offsets for the instructions that require and offset
//PC is the PC at the time that the instruction is executed
//offsetLength is the length in bits that the instruction will be in the instruction
uint16_t parse_pc_offset(char *buffer, uint16_t pc, short offsetLength);
uint16_t parse_r_offset(char *buffer);
uint16_t string_to_uint16(char *str, char mode);

//Intstructions to parse the operands for each of the
//LC-3 assembly language instructions. The instruction
//passed as an argument should have bot the bits set for
//the opcode and any constant bits set.
//If there was an error while the parsing the function
//returns an int corresponding to the error.
//A returned value of 0 indicates no error.
int parse_add(uint16_t *instruction, uint16_t lc);
#define parse_and parse_add
int parse_br(uint16_t *instruction, uint16_t lc);
int parse_jmp(uint16_t *instruction, uint16_t lc);
int parse_jsr(uint16_t *instruction, uint16_t lc);
#define parse_jsrr parse_jmp
int parse_ld(uint16_t *instruction, uint16_t lc);
#define parse_ldi parse_ld
int parse_ldr(uint16_t *instruction, uint16_t lc);
#define parse_lea parse_ld
int parse_not(uint16_t *instruction, uint16_t lc);
#define parse_st parse_ld
#define parse_sti parse_ld
#define parse_str parse_ldr
int parse_trap(uint16_t *instruction, uint16_t lc);
#endif
