#include <stdlib.h>
#include <stdint.h>

#include "memory/instruction.h"
#include "cpu/register.h"
#include "disk/elf.h"

inst_t program[15] = {
    // uint64_t add(uint64_t, uint64_t)
    {
        // 3
        push_reg,
        {REG, 0, 0, (uint64_t *)&reg.rbp, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "push   \%rbp"
    },
    {   
        // 4
        mov_reg_reg,
        {REG, 0, 0, &reg.rsp, NULL},
        {REG, 0, 0, &reg.rbp, NULL},
        "mov    \%rsp,\%rbp"
    },
    {
        // 5
        mov_reg_mem,
        {REG, 0, 0, (uint64_t *)&reg.rdi, NULL},
        {MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL},
        "mov    \%rdi,0x18(\%rbp)"   
    },
    {
        // 6
        mov_reg_mem,
        {REG, 0, 0, (uint64_t *)&reg.rsi, NULL},
        {MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp, NULL},
        "mov    \%rsi,-0x20(\%rbp)"
    },
    {
        // 7
        mov_mem_reg,
        {MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL},
        {REG, 0, 0, (uint64_t *)&reg.rdx, NULL},
        "mov    -0x18(\%rbp),\%rdx"
    },
    {
        // 8
        mov_mem_reg,
        {MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp, NULL},
        {REG, 0, 0, (uint64_t *)&reg.rax, NULL},
        "mov    -0x20(\%rbp),\%rax"
    },
    {
        // 9
        add_reg_reg,
        {REG, 0, 0, (uint64_t *)&reg.rdx, NULL},
        {REG, 0, 0, (uint64_t *)&reg.rax, NULL},
        "add    \%rdx,\%rax"
    },
    {
        // 10
        mov_reg_mem,
        {REG, 0, 0, (uint64_t *)&reg.rax, NULL},
        {MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL},
        "mov    \%rax,-0x8(\%rbp)"
    },
    {
        // 11
        mov_mem_reg,
        {MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL},
        {REG, 0, 0 , (uint64_t *)&reg.rax, NULL},
        "mov    -0x8(\%rbp),\%rax"
    },
    {
        // 12
        pop_reg, 
        {REG, 0, 0, (uint64_t *)&reg.rbp, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "pop     \%rbp"
    },
    {
        // 13
        ret, 
        {EMPTY, 0, 0, NULL, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "retq"
    },
    // main entry point
    {   
        // 0
        mov_reg_reg,
        {REG, 0, 0, &reg.rdx, NULL},
        {REG, 0, 0, &reg.rsi, NULL},
        "mov    \%rdx,\%rsi"
    },
    {
        // 1
        mov_reg_reg,
        {REG, 0, 0, (uint64_t *)&reg.rax, NULL},
        {REG, 0, 0, (uint64_t *)&reg.rdi, NULL},
        "mov    \%rax,\%rdi"
    },
    {   
        // 2
        call,
        {IMM, (uint64_t)&(program[0]), 0, NULL, NULL},
        {EMPTY, 0, 0, NULL, NULL},
        "callq  <add>"
    },
    {
        // 14
        mov_reg_mem,
        {REG, 0, 0, (uint64_t *)&reg.rax, NULL},
        {MM_IMM_REG, -0x8, 0, (uint64_t *)&reg.rbp, NULL},
        "mov    \%rax,-0x8(\%rbp)"
    }
};