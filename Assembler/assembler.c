#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "symboltable.h"
#include "opcodetable.h"
#include "parser.h"

unsigned short address;

char *token;
unsigned short location_counter;
unsigned short instruction;
FILE *fout;
char *asmfilename, *objfilename, *symfilename;

//This function is used to output words to the object file
int writeWord(uint16_t word) {
	//Words are output in little endian format for compatibility with other LC-3 simulators
	//word = (word << 8) | (word >> 8);
	return fwrite(&word, sizeof(word), 1, fout);
}

void proc_args(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Enter the name of the program to assemble in the command line.\n");
		exit(1);
	} else if (argc == 2) {
		asmfilename = argv[1];
	} else {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-i") == 0) {
				i++;
				asmfilename = argv[i];
			} else if (strcmp(argv[i], "-o") == 0) {
				i++;
				objfilename = argv[i];
			} else if (strcmp(argv[i], "-s") == 0) {
				i++;
				symfilename = argv[i];
			}
		}
	}
	if (asmfilename == NULL) {
		printf("Error: No asm file specified!\n");
		exit(1);
	}
	if (objfilename == NULL) {
		objfilename = malloc(strlen(asmfilename) + 1);
		int i = strlen(asmfilename) - 3;
		strncpy(objfilename, asmfilename, i);
		strncpy(objfilename + i, "obj", 4);
	}
	if (symfilename == NULL) {
		symfilename = malloc(strlen(asmfilename) + 1);
		int i = strlen(asmfilename) - 3;
		strncpy(symfilename, asmfilename, i);
		strncpy(symfilename + i, "sym", 4);
	}
	printf("%s, %s, %s\n", asmfilename, objfilename, symfilename);
}

int main(int argc, char *argv[]) {
	printf("Starting program\n");
	proc_args(argc, argv);

	address = 0x3000;

	set_input_stream(asmfilename);

	//First pass
	printf("Starting first pass\n");
	next_token(&token);
	if (strcmp(token, ".ORIG") != 0) {
		printf("Error: .ORIG not found at the start of the file!\nAbort!\n");
		return 1;
	}
	free(token);

	next_token(&token);
	if (token[0] == '#') address = stringToShort(token+1, '#');
	else if (token[0] == 'x' || token[0] == 'X') address = stringToShort(token+1, 'x');
	else {
		printf("Error: .ORIG address not formatted properly!\nAbort!\n");
		exit(1);
	}

	location_counter = address;
	free(token);

	while (1) {
		next_token(&token);

		if (strcmp(token, ".END") == 0) break;
		else if (token[0] == '.') {
			if (strcmp(token, ".ORIG") == 0) {
				printf("Error: .ORIG pseudo-op used more than once!\nAbort!\n");
				return 1;
			}
			if (strcmp(token, ".FILL") == 0) {
				location_counter++;
				free(token);
				next_token(&token);
				free(token);
				continue;
			} else if (strcmp(token, ".BLKW") == 0) {
				free(token);
				next_token(&token);
				location_counter += stringToShort(token, '#');
				free(token);
				continue;
			} else if (strcmp(token, ".STRINGZ") == 0) {
				free(token);
				next_token(&token);
				if (token[0] != '\"') {
					printf("Error: missing string literal after .STRINGZ\n");
					exit(1);
				}
				location_counter += strlen(token) - 1;
				free(token);
				continue;
			}
		} else if (!strcmp(token, "ADD") || !strcmp(token, "AND")
			|| !strcmp(token, "LDR") || !strcmp(token, "STR")) {
				free(token);
				next_token(&token);
				free(token);
				next_token(&token);
				free(token);
				next_token(&token);
				location_counter++;
		} else if (!strcmp(token, "LD") || !strcmp(token, "LDI") || !strcmp(token, "ST")
			|| !strcmp(token, "STI") || !strcmp(token, "LEA") || !strcmp(token, "NOT")) {
				free(token);
				next_token(&token);
				free(token);
				next_token(&token);
				location_counter++;
		} else if (!strcmp(token, "BRn") || !strcmp(token, "BRz") || !strcmp(token, "BRp")
			|| !strcmp(token, "BRnz") || !strcmp(token, "BRnp") || !strcmp(token, "BRnzp")
			|| !strcmp(token, "BR") || !strcmp(token, "JMP") || !strcmp(token, "JSR")
			|| !strcmp(token, "JSRR") || !strcmp(token, "TRAP")) {
				free(token);
				next_token(&token);
				location_counter++;
		} else if (!strcmp(token, "RET") || !strcmp(token, "RTI") || !strcmp(token, "HALT")
			|| !strcmp(token, "GETC") || !strcmp(token, "OUT") || !strcmp(token, "PUTS")
			|| !strcmp(token, "IN") || !strcmp(token, "PUTSP")) { location_counter++;
		} else if (findTableEntry(token) != NULL) {
			printf("Label %s used more than once!\nAbort!\n", token);
			//return 1;
		} else addTableEntry(token, location_counter);
		free(token);
	}
	printf("Finished first pass %d\n", line_number);
	print_symbol_table(symfilename);

	reset_tokenizer();
	fout = fopen(objfilename,"wb");
	// Second pass
	printf("Starting second pass\n");
	next_token(&token);
	if (strcmp(token, ".ORIG") != 0) {
		printf("Error: .ORIG not found at the start of the file!\nAbort!\n");
		return 1;
	}
	free(token);

	next_token(&token);
	if (token[0] == '#') address = stringToShort(token+1, '#');
	else if (token[0] == 'x' || token[0] == 'X') address = stringToShort(token+1, 'x');
	else {
		printf("Error: .ORIG address not formatted properly!\nAbort!\n");
		exit(1);
	}

	location_counter = address;
	writeWord(address);
	free(token);

	while (1) {
		next_token(&token);

		if (strcmp(token, ".END") == 0) break;
		else if (token[0] == '.') {
			if (strcmp(token, ".ORIG") == 0) {
				printf("Error: .ORIG pseudo-op used more than once!\nAbort!\n");
				return 1;
			}
			if (strcmp(token, ".FILL") == 0) {
				location_counter++;
				free(token);
				next_token(&token);
				ste *temp = findTableEntry(token);
				uint16_t fillvalue = temp != NULL ? temp->address : stringToShort(token+1, token[0]);
				writeWord(fillvalue);
				free(token);
				continue;
			} else if (strcmp(token, ".BLKW") == 0) {
				free(token);
				next_token(&token);
				for (int i = stringToShort(token, '#'); i > 0; i--) {
					location_counter++;
					writeWord(0);
				}
				free(token);
				continue;
			} else if (strcmp(token, ".STRINGZ") == 0) {
				free(token);
				next_token(&token);
				if (token[0] != '\"') {
					printf("Error: missing string literal after .STRINGZ\n");
					exit(1);
				}
				size_t length = strlen(token) - 1;
				for (int i = 1; i < length; i++) {
					uint16_t temp = (uint16_t) token[i];
					writeWord(temp);
					location_counter++;
				}
				writeWord(0);
				location_counter++;
				free(token);
				continue;
			}
		} else {
			opte *opcode_entry = search_optable(token);
			if (opcode_entry != NULL) {
				uint16_t instruction = opcode_entry->opcode;
				if (opcode_entry->parseFunc != NULL)
					opcode_entry->parseFunc(&instruction, location_counter);
				writeWord(instruction);
				location_counter++;
			}
		}
		free(token);
	}
	printf("Finished second pass\n");
	fclose(fout);
}
