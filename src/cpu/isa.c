#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <hearders/cpu.h>
#include <headers/memory.h>
#include <headers/common.h>

/* ======================================== */
/*      instruction set architecture        */
/* ======================================== */

typedef enum INST_OPERATOR
{
    INST_MOV,   // 0
    INST_PUSH,  // 1
    INST_POP,   // 2
    INST_LEAVE, // 3
    INST_CALL,  // 4
    INST_RET,   // 5
    INST_ADD,   // 6
    INST_SUB,   // 7
    INST_CMP,   // 8
    INST_JNE,   // 9
    INST_JMP,   // 10
} op_t;

typedef enum OPERAND_TYPE
{
    EMPTY,                 // 0
    IMM,                   // 1
    REG,                   // 2
    MM_IMM,                // 3
    MM_REG,                // 4
    MM_IMM_REG,            // 5
    MM_REG1_REG2,          // 6
    MM_IMM_REG1_REG2,      // 7
    MM_REG2_SCAL,          // 8
    MM_IMM_REG2_SCAL,      // 9
    MM_REG1_REG2_SCAL,     // 10
    MM_IMM_REG1_REG2_SCAL, // 11
} od_type_t;

typedef struct OPERAND_STRUCT
{
    od_type_t type;
    u_int64_t imm;
    u_int64_t scal;
    u_int64_t reg1;
    u_int64_t reg2;
} od_t;

typedef struct INST_STRUCT
{
    op_t op;
    od_t src;
    od_t dst;
} inst_t;

/* ======================================== */
/*      parse assembly instruction          */
/* ======================================== */

// function to map the string assembly code to inst_t instance
static void parse_instruction(const char *str, inst_t *inst, core_t *cr);
static void parse_operand(const char *str, od_t *od, core_t *cr);
static uint64_t decode_operand(od_t *od);

/// @brief interpret the operand
/// @param od operand pointer
/// @return uint64_t num
static uint64_t decode_od(od_t *od)
{
    if (od->type == IMM)
    {
        return *(uint64_t *)&od->imm;
    }
    else if (od->type == REG)
    {
        return od->reg1;
    }
    else if (od->type == EMPTY)
    {
        return 0;
    }

    else
    {
        // MM 9 type
        uint64_t vaddr = 0;

        if (od->type == MM_IMM)
        {
            vaddr = od->imm; // case 1
        }
        else if (od->type == MM_REG)
        {
            vaddr = *((uint64_t *)od->reg1); // case 2
        }
        else if (od->type == MM_IMM_REG)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1); // case 3
        }
        else if (od->type == MM_REG1_REG2)
        {
            vaddr = *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2); // case 4
        }
        else if (od->type == MM_IMM_REG1_REG2)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2); // case 5
        }
        else if (od->type == MM_REG2_SCAL)
        {
            vaddr = *((uint64_t *)od->reg1) * od->scal; // case 6
        }
        else if (od->type == MM_IMM_REG2_SCAL)
        {
            vaddr = od->imm + *((uint64_t *)od->reg2) * od->scal; // case 7
        }
        else if (od->type == MM_REG1_REG2_SCAL)
        {
            vaddr = *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2) * od->scal; // case 8
        }
        else if (od->type == MM_IMM_REG1_REG2_SCAL)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2) * od->scal; // case 9
        }

        return vaddr;
    }
    return 0;
}

static void parse_instruction(const char *str, inst_t *inst, core_t *cr)
{
}

static void parse_operand(const char *str, od_t *od, core_t *cr)
{
    // str : assembly code string
    // od : pointer to address to store the parsed operand
    // cr : active core processor
    od->type = EMPTY;
    od->imm = 0;
    od->scal = 0;
    od->reg1 = 0;
    od->reg2 = 0;

    int str_len = strlen(str);
    if (str_len == 0)
    {
        return;
    }
    if (str[0] == '$')
    {
        // immediate
        od->type = IMM;

        od->IMM = string2uint_range();
    }
    else if (str[0] == '%')
    {
        // register
    }
    else
    {
        // memory
    }
}

/* ======================================== */
/*           instruction handlers           */
/* ======================================== */

static void mov_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void pop_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void ret_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void add_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void sub_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void cmp_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void jne_handler(od_t *src_od, od_t *dst_od, core_t *cr);
static void jmp_handler(od_t *src_od, od_t *dst_od, core_t *cr);

// handler table storing the handlers to different instruction types
typedef void (*handler_t)(od_t *, od_t *, core_t *);

#define NUM_INSTRTYPE 11
/// @brief init handler table by handler func
static handler_t handler_table[NUM_INSTRTYPE] = {
    &mov_handler,   // 0
    &push_handler,  // 1
    &pop_handler,   // 2
    &leave_handler, // 3
    &call_handler,  // 4
    &ret_handler,   // 5
    &add_handler,   // 6
    &sub_handler,   // 7
    &cmp_handler,   // 8
    &jne_handler,   // 9
    &jmp_handler,   // 10
};

/// @brief reset the condition flags
static inline void reset_cflags(core_t *cr)
{
    cr->CF = 0;
    cr->ZF = 0;
    cr->SF = 0;
    cr->OF = 0;
}

#define MAX_INSTRUCTION_CHAR 64
/// @brief update rip pointer to the next instruction sequentially
static inline void next_rip(core_t *cr)
{
    cr->rip = cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR;
}

// instruction handlers

static void mov_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG)
    {
        // src : register
        // dst : register
        *(uint64_t *)dst = *(uint64_t *)src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == REG && dst_od->type >= MM_IMM)
    {
        // src : register
        // dst : mem virtual address
        // write64bits_dram(va2pa(dst, cr), *(uint64_t *)src, cr);
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type >= MM_IMM && dst_od->type == REG)
    {
        // src : mem virtual address
        // dst : register
        // *(uint64_t *)dst = read64bits_dram(va2pa(src, cr), cr);
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == IMM && dst_od->type == REG)
    {
        // src : immediate number
        // dst : register
        *(uint64_t *)dst = src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG)
    {
        // src : register
        // dst : empty
        cr->reg.rsp = (cr->reg).rsp - 8;
        // write64bits_dram
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);
    if (src_od->type == REG)
    {
        // src : register
        // dst : empty
        // uint64_t old_val = read64bits_dram()
        (cr->reg).rsp += 8;
        *(uint64_t *)src = old_val;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);
}

static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // src : immediate number(virtual address of target function starting)
    // dst : empty
    (cr->reg).rsp -= 8;
    // write64bits_dram()
}

/// @brief instruction cycle is implemented in CPU
void instruction_cycle(core_t *cr)
{
    // fetch: get the instruction string by program counter
    const char *inst_str = (const char *)cr->rip;
    debug_printf(DEBUG_INSTRUCTIONCYCLE, "%lx    %s\n", cr->rip, inst_str);

    // decode: decode the run-time instruction operands
    inst_t inst;
    parse_instruction(inst_str, &inst, cr);

    // execute: get the function pointer or handler by the operator
    handler_t handler = handler_table[inst.op];

    // update: update CPU and memory according the instruction
    handler(&(inst.src), &(inst.dst), cr);
}

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