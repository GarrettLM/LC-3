#include <stdlib.h>
#include <string.h>
#include "opcodetable.h"
#include "parser.h"

#define NUMBER_OF_MNEM 29

static opte optable[NUMBER_OF_MNEM] = {
  {"ADD", 0x1000, 3, parseADD},
  {"AND", 0x5000, 3, parseAND},
  {"BRn", 0x0800, 1, parseBR},
  {"BRp", 0x0200, 1, parseBR},
  {"BRz", 0x0400, 1, parseBR},
  {"BRnp", 0x0A00, 1, parseBR},
  {"BRnz", 0x0C00, 1, parseBR},
  {"BRnzp", 0x0E00, 1, parseBR},
  {"BRzp", 0x0600, 1, parseBR},
  {"GETC", 0xF020, 0, NULL},
  {"HALT", 0xF025, 0, NULL},
  {"IN", 0xF023, 0, NULL},
  {"JMP", 0xC000, 1, parseJMP},
  {"JSR", 0x4800, 1, parseJSR},
  {"JSRR", 0x4000, 1, parseJSRR},
  {"LD", 0x2000, 2, parseLD},
  {"LDI", 0xA000, 2, parseLDI},
  {"LDR", 0x6000, 3, parseLDR},
  {"LEA", 0xE000, 2, parseLEA},
  {"NOT", 0x903F, 1, parseNOT},
  {"OUT", 0xF021, 0, NULL},
  {"PUTS", 0xF022, 0, NULL},
  {"PUTSP", 0xF024, 0, NULL},
  {"RET", 0xC1C0, 0, NULL},
  {"RTI", 0x8000, 0, NULL},
  {"ST", 0x3000, 2, parseST},
  {"STI", 0xB000, 2, parseSTI},
  {"STR", 0x7000, 3, parseSTR},
  {"TRAP", 0xF000, 1, parseTRAP},
};

opte *search_optable(char *mnem) {
  for (int i = 0; i < NUMBER_OF_MNEM; i++) {
    if (!strcmp(mnem, optable[i].mnem)) return optable + i;
  }
  return NULL;
}
