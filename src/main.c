#include <stdio.h>

#include "cpu/register.h"
#include "cpu/mmu.h"

#include "memory/instruction.h"
#include "memory/dram.h"

#include "disk/elf.h"

int main()
{
    init_handler_table();

    // init register
    reg.rax = 0x12340000;
    reg.rbx = 0x555555555180;
    reg.rcx = 0x555555555180;
    reg.rdx = 0xadcd;
    reg.rsi = 0x7fffffffde18;
    reg.rdi = 0x1;
    reg.rbp = 0x7fffffffdd20;
    reg.rsp = 0x7fffffffdd10;

    reg.rip = (uint64_t)&program[11];

    // inti memory

    write64bits_dram(va2pa(0x7fffffffdd30), 0x00007ffff7ffc620); // rbp
    write64bits_dram(va2pa(0x7fffffffdd28), 0x00007ffff7df1083);
    write64bits_dram(va2pa(0x7fffffffdd20), 0x00000000);
    write64bits_dram(va2pa(0x7fffffffdd18), 0x12340000);
    write64bits_dram(va2pa(0x7fffffffdd10), 0x0000abcd); // rsp

    // printf("%16lx\n", read64bits_dram(va2pa(0x7fffffffdd30)));
    // uint64_t pa = va2pa(0x7fffffffdd30);
    // printf("%16lx\n", *((uint64_t *)(&mm[pa])));

    // mm[va2pa(0x7fffffffdd30)] = 0x00007ffff7ffc620; // rbp
    // mm[va2pa(0x7fffffffdd28)] = 0x00007ffff7df1083;
    // mm[va2pa(0x7fffffffdd20)] = 0x00000000;
    // mm[va2pa(0x7fffffffdd18)] = 0x12340000;
    // mm[va2pa(0x7fffffffdd10)] = 0x0000abcd; // rsp

    // run instruction

    for (int i = 0; i < 15; ++i)
    {
        // instruction_cycle();
    }

    // verify

    int match = 1;
    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x555555555180);
    match = match && (reg.rcx == 0x555555555180);
    match = match && (reg.rdx == 0x12340000);
    match = match && (reg.rsi == 0xabcd);
    match = match && (reg.rdi == 0x12340000);
    match = match && (reg.rbp == 0x7fffffffdd20);
    match = match && (reg.rsp == 0x7fffffffdd10);

    if (match == 1)
    {
        printf("register match\n");
    }
    else
    {
        printf("register notmatch\n");
    }

    match = 1;
    match = match && (read64bits_dram(va2pa(0x7fffffffdd30)) == 0x00007ffff7ffc620); // rbp
    match = match && (read64bits_dram(va2pa(0x7fffffffdd28)) == 0x00007ffff7df1083);
    match = match && (read64bits_dram(va2pa(0x7fffffffdd20)) == 0x00000000);
    match = match && (read64bits_dram(va2pa(0x7fffffffdd18)) == 0x0000abcd);
    match = match && (read64bits_dram(va2pa(0x7fffffffdd10)) == 0x0000abcd); // rsp

    if (match == 1)
    {
        printf("memory match\n");
    }
    else
    {
        printf("memory notmatch\n");
    }
    return 0;
}
