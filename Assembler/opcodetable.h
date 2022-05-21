#ifndef OPCODE_FILE
#define OPCODE_FILE
#include <stdint.h>

struct optableentry {
  char *mnem;
  uint16_t opcode;
  uint16_t arguments;
  int (*parseFunc)(uint16_t *instruction, uint16_t lc);
};

typedef struct optableentry opte;

opte *search_optable(char *mnem);

#endif
