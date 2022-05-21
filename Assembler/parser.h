#ifndef PARSER_FILE
#define PARSER_FILE

//These constants are passed to the parseOperand function
//to indicate the operand type to parse
#define DR 0	//Parse the destination register (this is also used for the source register for the load instruction)
#define SRONE 1	//Parse the first source register for an instruction
#define OPERAND_TWO 2	//Parse the second operand for an instruction (this could be a register or an immediate value)

//Buffer is a string representation of a register, address, or immediate value to be parsed
short parseOperand(char *buffer, short operandType);
//This is used to parse PC offsets for the instructions that require and offset
//PC is the PC at the time that the instruction is executed
//offsetLength is the length in bits that the instruction will be in the instruction
short parsePCOffset(char *buffer, short PC, short offsetLength);
short parseROffset(char *buffer);
short stringToShort(char *str, char mode);

int parseAdd(uint16_t *instruction, uint16_t lc);
int parseAnd(uint16_t *instruction, uint16_t lc);
int parseBr(uint16_t *instruction, uint16_t lc);
int parseJmp(uint16_t *instruction, uint16_t lc);
int parseJsr(uint16_t *instruction, uint16_t lc);
int parseJsrr(uint16_t *instruction, uint16_t lc);
int parseLd(uint16_t *instruction, uint16_t lc);
int parseLdi(uint16_t *instruction, uint16_t lc);
int parseLdr(uint16_t *instruction, uint16_t lc);
int parseLea(uint16_t *instruction, uint16_t lc);
int parseNot(uint16_t *instruction, uint16_t lc);
int parseSt(uint16_t *instruction, uint16_t lc);
int parseSti(uint16_t *instruction, uint16_t lc);
int parseStr(uint16_t *instruction, uint16_t lc);
int parseTrap(uint16_t *instruction, uint16_t lc);
#endif
