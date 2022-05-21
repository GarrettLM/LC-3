#ifndef MEMORY_MANAGEMENT_UNIT_FILE
#define MEMORY_MANAGEMENT_UNIT_FILE
#include <stdint.h>
#include <stdio.h>

void loadFile(uint16_t startAddress, FILE *fin);
uint16_t getMA(uint16_t mar);
void setMA(uint16_t mar, uint16_t mdr);
#endif
