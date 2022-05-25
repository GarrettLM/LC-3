#include <stdlib.h>
#include <string.h>
#include "opcodetable.h"
#include "parser.h"

#define NUMBER_OF_MNEM 29

static opte optable[NUMBER_OF_MNEM] = {
  {"ADD", 0x1000, 3, parse_add},
  {"AND", 0x5000, 3, parse_and},
  {"BRn", 0x0800, 1, parse_br},
  {"BRp", 0x0200, 1, parse_br},
  {"BRz", 0x0400, 1, parse_br},
  {"BRnp", 0x0A00, 1, parse_br},
  {"BRnz", 0x0C00, 1, parse_br},
  {"BRnzp", 0x0E00, 1, parse_br},
  {"BRzp", 0x0600, 1, parse_br},
  {"GETC", 0xF020, 0, NULL},
  {"HALT", 0xF025, 0, NULL},
  {"IN", 0xF023, 0, NULL},
  {"JMP", 0xC000, 1, parse_jmp},
  {"JSR", 0x4800, 1, parse_jsr},
  {"JSRR", 0x4000, 1, parse_jsrr},
  {"LD", 0x2000, 2, parse_ld},
  {"LDI", 0xA000, 2, parse_ldi},
  {"LDR", 0x6000, 3, parse_ldr},
  {"LEA", 0xE000, 2, parse_lea},
  {"NOT", 0x903F, 1, parse_not},
  {"OUT", 0xF021, 0, NULL},
  {"PUTS", 0xF022, 0, NULL},
  {"PUTSP", 0xF024, 0, NULL},
  {"RET", 0xC1C0, 0, NULL},
  {"RTI", 0x8000, 0, NULL},
  {"ST", 0x3000, 2, parse_st},
  {"STI", 0xB000, 2, parse_sti},
  {"STR", 0x7000, 3, parse_str},
  {"TRAP", 0xF000, 1, parse_trap},
};

opte *search_optable(char *mnem) {
  for (int i = 0; i < NUMBER_OF_MNEM; i++) {
    if (!strcmp(mnem, optable[i].mnem)) return optable + i;
  }
  return NULL;
}
