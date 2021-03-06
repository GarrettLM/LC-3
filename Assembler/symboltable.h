#ifndef SYMBOL_FILE
#define SYMBOL_FILE
#include <stdint.h>

#define TABLE_SIZE 1024

struct symboltableentry {
	uint16_t address;		//This is the absolute address that the symbol represents
	char *symbol;
};

typedef struct symboltableentry ste;

ste *search_symtable(char *symbol);
int add_sym_entry(char *symbol, uint16_t address);
int print_symtable(char *symfilename);

#endif
