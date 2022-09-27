#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <headers/memory.h>
#include <headers/cpu.h>
#include <headers/common.h>

#define SRAM_CACHE_SETTING 0 // 0 mains open read&write

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
        val += (((uint64_t)mm[paddr + i]) << (8 * i));
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
        mm[paddr + i] = (data >> (8 * i)) & 0xff;
    }
}
void readinst_dram(uint64_t paddr, char *buf, core_t *cr)
{
    for (int i = 0; i < MAX_INSTRUCTION_CHAR; ++i)
    {
        buf[i] = (char)pm[paddr + i];
    }
}

void writeinst_dram(uint64_t paddr, const char *str, core_t *cr)
{
    int len = strlen(str);
    assert(len < MAX_INSTRUCTION_CHAR);

    for (int i = 0; i < MAX_INSTRUCTION_CHAR; ++i)
    {
        if (i < len)
        {
            pm[paddr + i] = (uint8_t)str[i];
        }
        else
        {
            pm[paddr + i] =
        }
    }
}

// void print_register()
// {
//     printf("rax = %16lx\trbx = %16lx\trcx = %16lx\trdx = %16lx\n",
//            reg.rax, reg.rbx, reg.rcx, reg.rdx);
//     printf("rsi = %16lx\trdi = %16lx\trbp = %16lx\trsp = %16lx\n",
//            reg.rsi, reg.rdi, reg.rbp, reg.rsp);
//     printf("rip = %16lx\n", reg.rip);
//     return;
// }

// void print_stack()
// {
//     int curr = 10;
//     uint64_t *high = (uint64_t *)&mm[va2pa(reg.rsp)];
//     high = &high[curr];

//     uint64_t rsp_start = reg.rsp + curr * 8;
//     for (int i = 0; i < curr * 2; ++i)
//     {
//         uint64_t *curr_ptr = (uint64_t *)(high - i);
//         printf("0x%16lx : %16lx", rsp_start, (uint64_t)*curr_ptr);

//         if (i == curr)
//         {
//             printf(" <== rsp");
//         }

//         rsp_start -= 8;

//         printf("\n");
//     }
//     return;
// }