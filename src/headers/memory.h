#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <headers/cpu.h>

#define PHYSICAL_MEMORY_SPACE 65536
#define MAX_INDEX_PHYSICAL_PAGE 15

// physical memory
// 16 physical memory pages
uint8_t pm[PHYSICAL_MEMORY_SPACE];

// read write bytes by instruction to dram
uint64_t read64bits_dram(uint64_t paddr, core_t *cr);
void write64bits_dram(uint64_t paddr, uint64_t data, core_t *cr);

// read write instruction to dram
void readinst_dram(uint64_t paddr, char *buf, core_t *cr);
void writeinst_dram(uint64_t paddr, const char *str, core_t *cr);

#endif
