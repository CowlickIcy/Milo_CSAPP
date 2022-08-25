#include "memory/dram.h"

#define SRAM_CACHE_SETTING 1

uint64_t read64bits_dram(uint64_t paddr)
{
    if (SRAM_CACHE_SETTING == 1)
    {
        return 0x0;
    }

    uint64_t val = 0x0;
    // fix uint8_t * 8 to uint64_t
    // 0x**** **** **** **11
    // 0x**** **** **** 22**
    // ......
    // 0x88** **** **** ****
    for (int i = 0; i < 8; ++i)
    {
        val += (((uint64_t)mm[paddr + i]) << 8 * i);
    }
    return 0x0;
}
void write64bits_dram(uint64_t paddr, uint64_t data)
{
    if (SRAM_CACHE_SETTING == 1)
    {
        return;
    }
    // fix uint64_t to uint8_t * 8
    // 0x**** **** **** **11
    // 0x**** **** **** 22**
    // ......
    // 0x88** **** **** ****
    for (int i = 0; i < 8; ++i)
    {
        mm[paddr + i] = (data >> 8 * i) & 0xff;
    }
}

void print_register()
{
    return;
}

void print_stack()
{
    return;
}