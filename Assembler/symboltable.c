#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symboltable.h"

static ste symbolTable[TABLE_SIZE];
static int table_ptr;	//This is a pointer to the first empty entry of the symble table
					//If table_ptr == TABLE_SIZE the table is full

ste *findTableEntry(char *symbol) {
	for (int i = 0; i < table_ptr; i++)
		if (!strcmp(symbolTable[i].symbol, symbol)) return symbolTable + i;
	return NULL;
}

int addTableEntry(char *symbol, uint16_t address) {
	if (table_ptr == TABLE_SIZE) return -1;
	symbolTable[table_ptr].address = address;
	size_t symlen = strchr(symbol, ':') ? strlen(symbol) : strlen(symbol) + 1;
	symbolTable[table_ptr].symbol = malloc(symlen);
	strncpy(symbolTable[table_ptr].symbol, symbol, symlen);
	table_ptr++;
	return TABLE_SIZE - table_ptr;
}

int print_symbol_table(char *symfilename) {
	printf("Printing symbol file to: %s\n", symfilename);
	FILE *symbolfile = fopen(symfilename, "w");
	if (symbolfile == NULL) {
		printf("Could not open sybmol file: %s\n", symfilename);
		return 0;
	}
	for (int i = 0; i < table_ptr; i++) {
		ste te = symbolTable[i];
		fprintf(symbolfile, "%x    |    %s\n", te.address, te.symbol);
	}
	fclose(symbolfile);
	return 1;
}
