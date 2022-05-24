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
uint16_t parse_PCoffset(char *buffer, uint16_t PC, short offsetLength);
uint16_t parse_Roffset(char *buffer);
short stringToShort(char *str, char mode);

//Intstructions to parse the operands for each of the
//LC-3 assembly language instructions. The instruction
//passed as an argument should have bot the bits set for
//the opcode and any constant bits set.
//If there was an error while the parsing the function
//returns an int corresponding to the error.
//A returned value of 0 indicates no error.
int parseADD(uint16_t *instruction, uint16_t lc);
int parseAND(uint16_t *instruction, uint16_t lc);
int parseBR(uint16_t *instruction, uint16_t lc);
int parseJMP(uint16_t *instruction, uint16_t lc);
int parseJSR(uint16_t *instruction, uint16_t lc);
int parseJSRR(uint16_t *instruction, uint16_t lc);
int parseLD(uint16_t *instruction, uint16_t lc);
int parseLDI(uint16_t *instruction, uint16_t lc);
int parseLDR(uint16_t *instruction, uint16_t lc);
int parseLEA(uint16_t *instruction, uint16_t lc);
int parseNOT(uint16_t *instruction, uint16_t lc);
int parseST(uint16_t *instruction, uint16_t lc);
int parseSTI(uint16_t *instruction, uint16_t lc);
int parseSTR(uint16_t *instruction, uint16_t lc);
int parseTRAP(uint16_t *instruction, uint16_t lc);
#endif
