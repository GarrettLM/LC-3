#ifndef MEMORY_MANAGEMENT_UNIT_FILE
#define MEMORY_MANAGEMENT_UNIT_FILE
#include <stdint.h>
#include <stdio.h>

void load_file(uint16_t startAddress, FILE *fin);
uint16_t get_mem(uint16_t mar);
void set_mem(uint16_t mar, uint16_t mdr);
#endif
