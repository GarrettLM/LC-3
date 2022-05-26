#ifndef CONSOLE_IO_FILE
#define CONSOLE_IO_FILE
#include <stdint.h>

uint16_t get_kbsr();
uint16_t get_kbdr();
uint16_t get_dsr();
uint16_t set_ddr(uint16_t c);

#endif
