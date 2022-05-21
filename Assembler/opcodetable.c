#include <stdlib.h>
#include <string.h>
#include "opcodetable.h"
#include "parser.h"

#define NUMBER_OF_MNEM 29

static opte optable[NUMBER_OF_MNEM] = {
  {"ADD", 0x1000, 3, parseAdd},
  {"AND", 0x5000, 3, parseAnd},
  {"BRn", 0x0800, 1, parseBr},
  {"BRp", 0x0200, 1, parseBr},
  {"BRz", 0x0400, 1, parseBr},
  {"BRnp", 0x0A00, 1, parseBr},
  {"BRnz", 0x0C00, 1, parseBr},
  {"BRnzp", 0x0E00, 1, parseBr},
  {"BRzp", 0x0600, 1, parseBr},
  {"GETC", 0xF020, 0, NULL},
  {"HALT", 0xF025, 0, NULL},
  {"IN", 0xF023, 0, NULL},
  {"JMP", 0xC000, 1, parseJmp},
  {"JSR", 0x4800, 1, parseJsr},
  {"JSRR", 0x4000, 1, parseJsrr},
  {"LD", 0x2000, 2, parseLd},
  {"LDI", 0xA000, 2, parseLdi},
  {"LDR", 0x6000, 3, parseLdr},
  {"LEA", 0xE000, 2, parseLea},
  {"NOT", 0x903F, 1, parseNot},
  {"OUT", 0xF021, 0, NULL},
  {"PUTS", 0xF022, 0, NULL},
  {"PUTSP", 0xF024, 0, NULL},
  {"RET", 0xC1C0, 0, NULL},
  {"RTI", 0x8000, 0, NULL},
  {"ST", 0x3000, 2, parseSt},
  {"STI", 0xB000, 2, parseSti},
  {"STR", 0x7000, 3, parseStr},
  {"TRAP", 0xF000, 1, parseTrap},
};

opte *search_optable(char *mnem) {
  for (int i = 0; i < NUMBER_OF_MNEM; i++) {
    if (!strcmp(mnem, optable[i].mnem)) return optable + i;
  }
  return NULL;
}
