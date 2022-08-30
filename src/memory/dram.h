#ifndef DRAM_H
#define DRAM_H

#include <stdint.h>

#define MM_LEM 1000

// physical memory
uint8_t mm[MM_LEM];     

// virtual address = 0 -> 0xffffffffffffffff
// physical adrress = 000 -> 1023

uint64_t read64bits_dram(uint64_t paddr);
void write64bits_dram(uint64_t paddr, uint64_t data);

void print_register();
void print_stack();
#endif