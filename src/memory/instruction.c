#include <stdio.h>

#include "memory/instruction.h"
#include "cpu/mmu.h"
#include "cpu/register.h"
#include "memory/dram.h"

static uint64_t decode_od(od_t od)
{
    if (od.type == IMM)
    {
        return od.imm;
    }
    else if (od.type == REG)
    {
        return (uint64_t)od.reg1;
    }
    else
    {
        // MM 9 type
        uint64_t vaddr = 0;

        if (od.type == IMM)
        {
            vaddr = *((uint64_t *)&od.imm); // case 1
        }
        else if (od.type == MM_REG)
        {
            vaddr = *(od.reg1); // case 2
        }
        else if (od.type == MM_IMM_REG)
        {
            vaddr = od.imm + *(od.reg1); // case 3
        }
        else if (od.type == MM_REG1_REG2)
        {
            vaddr = *(od.reg1) + *(od.reg2); // case 4
        }
        else if (od.type == MM_IMM_REG1_REG2)
        {
            vaddr = od.imm + *(od.reg1) + *(od.reg2); // case 5
        }
        else if (od.type == MM_REG2_SCAL)
        {
            vaddr = *(od.reg2) * od.scal; // case 6`
        }
        else if (od.type == MM_IMM_REG2_SCAL)
        {
            vaddr = od.imm + *(od.reg2) * od.scal; // case 7
        }
        else if (od.type == MM_REG1_REG2_SCAL)
        {
            vaddr = *(od.reg1) + *(od.reg2) * od.scal; // case 8
        }
        else if (od.type == MM_IMM_REG1_REG2_SCAL)
        {
            vaddr = od.imm + *(od.reg1) + *(od.reg2) * od.scal; // case 9
        }

        return vaddr;
    }
}

void instruction_cycle()
{
    inst_t *instr = (inst_t *)reg.rip; // physical addr
    // inst_t instr = program[reg.rip];   // ptr
    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);

    handler_t handler = handler_table[instr->op];

    // add_reg_reg_handler(src = &rax, dst = &rbx)
    handler(src, dst);

    printf("    %s\n", instr->code);
}

// function table
void init_handler_table()
{
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
    handler_table[call] = &call_handler;
    handler_table[push_reg] = &push_reg_handler;
    handler_table[pop_reg] = &pop_reg_handler;
    handler_table[mov_reg_mem] = &mov_reg_mem_handler;
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void add_reg_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = *(uint64_t *)dst + *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst)
{
    // src : imm address of called function
    // rsp = rsp - 8
    // 2. wirte return address to rsp memory and goto next instrcution
    reg.rsp -= 0x8;
    write64bits_dram(va2pa(reg.rsp), reg.rip + sizeof(inst_t));
    reg.rip = src;
}

void push_reg_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : empty
    // rsp = rsp - 8
    reg.rsp -= 0x8;
    write64bits_dram(va2pa(reg.rsp), *(uint64_t *)src);
    reg.rip = reg.rip + sizeof(inst_t);
}

void pop_reg_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : empty
    // rsp = rsp - 8
    reg.rsp -= 0x8;

    reg.rip = reg.rip + sizeof(inst_t);
}

void mov_reg_mem_handler(uint64_t src, uint64_t dst)
{
    // src : reg
    // dst : mem virtual address
    write64bits_dram(va2pa(dst),*(uint64_t *)src);

    reg.rip = reg.rip + sizeof(inst_t);
}