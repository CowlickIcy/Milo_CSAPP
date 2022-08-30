#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdlib.h>
#include <stdint.h>

#define NUM_INSTRTYPE 30

typedef enum OP
{
    mov_reg_reg,    // 0
    mov_reg_mem,    // 1
    mov_mem_reg,    // 2
    push_reg,       // 3
    pop_reg,        // 4
    call,           // 5
    ret,            // 6
    add_reg_reg,    // 7
    OP_NUM          // 8
} op_t;

typedef enum OD_TYPE
{
    EMPTY,
    IMM, REG,
    MM_IMM, MM_REG, MM_IMM_REG, MM_REG1_REG2, MM_IMM_REG1_REG2,
    MM_REG2_SCAL, MM_IMM_REG2_SCAL, MM_REG1_REG2_SCAL, MM_IMM_REG1_REG2_SCAL,
    OD_TYPE_NUM // 11
} od_type_t;

typedef struct OD
{
    od_type_t type;     // operand type

    int64_t imm;        // immediate num
    int64_t scal;       // scal num
    uint64_t *reg1;     // register 1
    uint64_t *reg2;     // register 2
} od_t;

typedef struct INSTRUCT_STRUCT
{
    op_t op;
    od_t src;
    od_t dst;
    char code[100]; // isntruction
} inst_t;

typedef void (*handler_t)(uint64_t, uint64_t);

handler_t handler_table[NUM_INSTRTYPE];

void init_handler_table();

void instruction_cycle();

void add_reg_reg_handler(uint64_t src, uint64_t dst);
void mov_reg_reg_handler(uint64_t src, uint64_t dst);
void call_handler(uint64_t src, uint64_t dst);
void ret_handler(uint64_t src, uint64_t dst);
void push_reg_handler(uint64_t src, uint64_t dst);
void pop_reg_handler(uint64_t src, uint64_t dst);
void mov_reg_mem_handler(uint64_t src, uint64_t dst);
void mov_mem_reg_handler(uint64_t src, uint64_t dst);

#endif