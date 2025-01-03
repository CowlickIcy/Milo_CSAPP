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
    reg.rbx = 0x0;
    reg.rcx = 0x8000660;
    reg.rdx = 0xadcd;
    reg.rsi = 0x7ffffffee2f8;
    reg.rdi = 0x1;
    reg.rbp = 0x7ffffffee210;
    reg.rsp = 0x7ffffffee1f0;

    reg.rip = (uint64_t)&program[11];

    // inti memory
    write64bits_dram(va2pa(0x7ffffffee210), 0x8000660); // rbp
    write64bits_dram(va2pa(0x7ffffffee208), 0x0);
    write64bits_dram(va2pa(0x7ffffffee200), 0xabcd);
    write64bits_dram(va2pa(0x7ffffffee1f8), 0x12340000);
    write64bits_dram(va2pa(0x7ffffffee1f0), 0x8000660); // rsp

    printf("Init register and stack.\n");
    print_register();
    print_stack();
    // printf("%16lx\n", read64bits_dram(va2pa(0x7fffffffdd30)));
    // uint64_t pa = va2pa(0x7fffffffdd30);
    // printf("%16lx\n", *((uint64_t *)(&mm[pa])));

    // mm[va2pa(0x7fffffffdd30)] = 0x00007ffff7ffc6120; // rbp
    // mm[va2pa(0x7fff  ffffdd28)] = 0x00007ffff7df1083;
    // mm[va2pa(0x7fffffffdd20)] = 0x00000000;
    // mm[va2pa(0x7fffffffdd18)] = 0x12340000;
    // mm[va2pa(0x7fffffffdd10)] = 0x0000abcd; // rsp

    // run instruction

    for (int i = 0; i < 15; ++i)
    {
        printf("No.%d instruction\n", i);
        instruction_cycle();

        print_register();
        print_stack();
    }

    // verify

    int match = 1;
    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x0);
    match = match && (reg.rcx == 0x8000660);
    match = match && (reg.rdx == 0xabcd);
    match = match && (reg.rsi == 0x7ffffffee2f8);
    match = match && (reg.rdi == 0x1);
    match = match && (reg.rbp == 0x7ffffffee210);
    match = match && (reg.rsp == 0x7ffffffee1f0);

    if (match == 1)
    {
        printf("register match\n");
    }
    else
    {
        printf("register notmatch\n");
    }

    match = 1;
    match = match && (read64bits_dram(va2pa(0x7ffffffee210)) == 0x8000660); // rbp
    match = match && (read64bits_dram(va2pa(0x7ffffffee208)) == 0x0);
    match = match && (read64bits_dram(va2pa(0x7ffffffee200)) == 0xabcd);
    match = match && (read64bits_dram(va2pa(0x7ffffffee1f8)) == 0x12340000);
    match = match && (read64bits_dram(va2pa(0x7ffffffee1f0)) == 0x8000660); // rsp
    
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
