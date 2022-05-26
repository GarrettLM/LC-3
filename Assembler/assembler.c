#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenizer.h"
#include "symboltable.h"
#include "opcodetable.h"
#include "parser.h"

char *token;
uint16_t location_counter;
uint16_t instruction;
FILE *fout;
char *asmfilename, *objfilename, *symfilename;

// This function is used to output words to the object file
int write_word(uint16_t word) {
	// Words are output in little endian format for compatibility with other LC-3 simulators
	word = (word << 8) | (word >> 8);
	return fwrite(&word, sizeof(word), 1, fout);
}

// A function to process the command line arguments
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
}

int main(int argc, char *argv[]) {
	printf("Starting program\n");
	proc_args(argc, argv);

	location_counter = 0x3000;

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
	if (token[0] == '#') location_counter = string_to_uint16(token+1, '#');
	else if (token[0] == 'x' || token[0] == 'X') location_counter = string_to_uint16(token+1, 'x');
	else {
		printf("Error: .ORIG address not formatted properly!\nAbort!\n");
		exit(1);
	}
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
				location_counter += string_to_uint16(token, '#');
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
		} else {
			opte *opcode_entry = search_optable(token);
			if (opcode_entry != NULL) {
				for (uint16_t i = opcode_entry->arguments; i > 0; i--) {
					free(token);
					next_token(&token);
				}
				location_counter++;
				free(token);
				continue;
			}
		}
		if (search_symtable(token) != NULL) {
			printf("Label %s used more than once!\nAbort!\n", token);
			//return 1;
		} else add_sym_entry(token, location_counter);
		free(token);
	}
	printf("Finished first pass\n");
	print_symtable(symfilename);

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
	if (token[0] == '#') location_counter = string_to_uint16(token+1, '#');
	else if (token[0] == 'x' || token[0] == 'X') location_counter = string_to_uint16(token+1, 'x');
	else {
		printf("Error: .ORIG address not formatted properly!\nAbort!\n");
		exit(1);
	}

	write_word(location_counter);
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
				ste *temp = search_symtable(token);
				uint16_t fillvalue = temp != NULL ? temp->address : string_to_uint16(token+1, token[0]);
				write_word(fillvalue);
				free(token);
				continue;
			} else if (strcmp(token, ".BLKW") == 0) {
				free(token);
				next_token(&token);
				for (int i = string_to_uint16(token, '#'); i > 0; i--) {
					location_counter++;
					write_word(0);
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
					write_word(temp);
					location_counter++;
				}
				write_word(0);
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
				write_word(instruction);
				location_counter++;
			}
		}
		free(token);
	}
	printf("Finished second pass\n");
	fclose(fout);
}
