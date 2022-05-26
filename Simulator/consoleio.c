#include <stdio.h>
#include "consoleio.h"

uint16_t get_kbsr() {
  return 0x1000;
}

uint16_t get_kbdr() {
  return getchar();
}

uint16_t get_dsr() {
  return 0x1000;
}

uint16_t set_ddr(uint16_t c) {
  uint16_t temp = (uint16_t) putchar((char) c);
  fflush(stdout);
  return temp;
}
