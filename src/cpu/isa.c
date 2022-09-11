#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <headers/cpu.h>
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
    EMPTY,                  // 0
    IMM,                    // 1
    REG,                    // 2
    MEM_IMM,                // 3
    MEM_REG,                // 4
    MEM_IMM_REG,            // 5
    MEM_REG1_REG2,          // 6
    MEM_IMM_REG1_REG2,      // 7
    MEM_REG2_SCAL,          // 8
    MEM_IMM_REG2_SCAL,      // 9
    MEM_REG1_REG2_SCAL,     // 10
    MEM_IMM_REG1_REG2_SCAL, // 11
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

        if (od->type == MEM_IMM)
        {
            vaddr = od->imm; // case 1
        }
        else if (od->type == MEM_REG)
        {
            vaddr = *((uint64_t *)od->reg1); // case 2
        }
        else if (od->type == MEM_IMM_REG)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1); // case 3
        }
        else if (od->type == MEM_REG1_REG2)
        {
            vaddr = *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2); // case 4
        }
        else if (od->type == MEM_IMM_REG1_REG2)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2); // case 5
        }
        else if (od->type == MEM_REG2_SCAL)
        {
            vaddr = *((uint64_t *)od->reg1) * od->scal; // case 6
        }
        else if (od->type == MEM_IMM_REG2_SCAL)
        {
            vaddr = od->imm + *((uint64_t *)od->reg2) * od->scal; // case 7
        }
        else if (od->type == MEM_REG1_REG2_SCAL)
        {
            vaddr = *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2) * od->scal; // case 8
        }
        else if (od->type == MEM_IMM_REG1_REG2_SCAL)
        {
            vaddr = od->imm + *((uint64_t *)od->reg1) + *((uint64_t *)od->reg2) * od->scal; // case 9
        }

        return vaddr;
    }
    return 0;
}

// register lookup table
static const char *reg_name_list[72] = {
    "%rax",
    "%eax",
    "%ax",
    "%ah",
    "%al",
    "%rbx",
    "%ebx",
    "%bx",
    "%bh",
    "%bl",
    "%rcx",
    "%ecx",
    "%cx",
    "%ch",
    "%cl",
    "%rdx",
    "%edx",
    "%dx",
    "%dh",
    "%dl",
    "%rsi",
    "%esi",
    "%si",
    "%sih",
    "%sil",
    "%rdi",
    "%edi",
    "%di",
    "%dih",
    "%dil",
    "%rbp",
    "%ebp",
    "%bp",
    "%bph",
    "%bpl",
    "%rsp",
    "%esp",
    "%sp",
    "%sph",
    "%spl",
    "%r8",
    "%r8d",
    "%r8w",
    "%r8b",
    "%r9",
    "%r9d",
    "%r9w",
    "%r9b",
    "%r10",
    "%r10d",
    "%r10w",
    "%r10b",
    "%r11",
    "%r11d",
    "%r11w",
    "%r11b",
    "%r12",
    "%r12d",
    "%r12w",
    "%r12b",
    "%r13",
    "%r13d",
    "%r13w",
    "%r13b",
    "%r14",
    "%r14d",
    "%r14w",
    "%r14b",
    "%r15",
    "%r15d",
    "%r15w",
    "%r15b",
};

static uint64_t reflect_register(const char *str, core_t *cr)
{
    reg_t *reg = &(cr->reg);
    // map table
    uint64_t reg_addr[72] = {
        (uint64_t) & (reg->rax),
        (uint64_t) & (reg->eax),
        (uint64_t) & (reg->ax),
        (uint64_t) & (reg->al),
        (uint64_t) & (reg->ah),
        (uint64_t) & (reg->rbx),
        (uint64_t) & (reg->ebx),
        (uint64_t) & (reg->bx),
        (uint64_t) & (reg->bl),
        (uint64_t) & (reg->bh),
        (uint64_t) & (reg->rcx),
        (uint64_t) & (reg->ecx),
        (uint64_t) & (reg->cx),
        (uint64_t) & (reg->cl),
        (uint64_t) & (reg->ch),
        (uint64_t) & (reg->rdx),
        (uint64_t) & (reg->edx),
        (uint64_t) & (reg->dx),
        (uint64_t) & (reg->dl),
        (uint64_t) & (reg->dh),
        (uint64_t) & (reg->rsi),
        (uint64_t) & (reg->esi),
        (uint64_t) & (reg->si),
        (uint64_t) & (reg->sil),
        (uint64_t) & (reg->sih),
        (uint64_t) & (reg->rdi),
        (uint64_t) & (reg->edi),
        (uint64_t) & (reg->di),
        (uint64_t) & (reg->dil),
        (uint64_t) & (reg->dih),
        (uint64_t) & (reg->rbp),
        (uint64_t) & (reg->ebp),
        (uint64_t) & (reg->bp),
        (uint64_t) & (reg->bpl),
        (uint64_t) & (reg->bph),
        (uint64_t) & (reg->rsp),
        (uint64_t) & (reg->eap),
        (uint64_t) & (reg->sp),
        (uint64_t) & (reg->spl),
        (uint64_t) & (reg->sph),
        (uint64_t) & (reg->r8),
        (uint64_t) & (reg->e8d),
        (uint64_t) & (reg->r8w),
        (uint64_t) & (reg->r8b),
        (uint64_t) & (reg->r9),
        (uint64_t) & (reg->e9d),
        (uint64_t) & (reg->r9w),
        (uint64_t) & (reg->r9b),
        (uint64_t) & (reg->r10),
        (uint64_t) & (reg->e10d),
        (uint64_t) & (reg->r10w),
        (uint64_t) & (reg->r10b),
        (uint64_t) & (reg->r11),
        (uint64_t) & (reg->e11d),
        (uint64_t) & (reg->r11w),
        (uint64_t) & (reg->r11b),
        (uint64_t) & (reg->r12),
        (uint64_t) & (reg->e12d),
        (uint64_t) & (reg->r12w),
        (uint64_t) & (reg->r12b),
        (uint64_t) & (reg->r13),
        (uint64_t) & (reg->e13d),
        (uint64_t) & (reg->r13w),
        (uint64_t) & (reg->r13b),
        (uint64_t) & (reg->r14),
        (uint64_t) & (reg->e14d),
        (uint64_t) & (reg->r14w),
        (uint64_t) & (reg->r14b),
        (uint64_t) & (reg->r15),
        (uint64_t) & (reg->e15d),
        (uint64_t) & (reg->r15w),
        (uint64_t) & (reg->r15b)
    } for (int i = 0; i < 72; ++i)
    {
        if (strcmp(stc, reg_name_list[i]) == 0)
        {
            return reg_addr[i];
        }
    }
    printf("parse register %s error\n", str);
    exit(0);
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

        od->IMM = string2uint_range(src, 1, -1);
        return;
    }
    else if (str[0] == '%')
    {
        // register
        od->type = REG;
        od->reg1 = reflect_register(src, cr);
    }
    else
    {
        // memory
        char imm[64] = {'\0'};
        int imm_len = 0;
        char reg1[64] = {'\0'};
        int reg1_len = 0;
        char reg2[64] = {'\0'};
        int reg2_len = 0;
        char scal[64] = {'\0'};
        int scal_len = 0;

        int ca = 0; // bracket
        int cb = 0; // comma

        for (int i = 0; i < str_len; ++i)
        {
            char c = str[i];
            if (c == '(' || c == ')')
            {
                ca++;
                continue;
            }
            else if (c == ',')
            {
                cb++;
                continue;
            }
            else
            {
                if (ca == 0)
                {
                    imm[imm_len] = c;
                    imm_le++;
                    continue;
                }
                else if (ca == 1)
                {
                    if (cb == 0)
                    {
                        // ???(xxxx)
                        // (xxxx)
                        reg1[reg1_len] = c;
                        reg1_len++ l;
                    }
                    else if (cb == 1)
                    {
                        // ???(???,xxxx)
                        reg2[reg2_len] = c;
                        reg2_len++;
                        continue;
                    }
                    else if (cb == 2)
                    {
                        // (???, ???, xxx)
                        scal[scal_len] = c;
                        scal_len++;
                    }
                }
            }
        }

        // imm, reg1, reg2, scal
        if (imm_len > 0)
        {
            od->imm = string2uint(imm);
            if (ca == 0)
            {
                od->type = MEM_IMM;
                return;
            }
        }
        if (scal_len > 0)
        {
            od->scal = string2uint(scal);
            if (od->scal != 1 && od->scal != 2 && od->scal != 4 && od->type != 8)
            {
                printf("%s is not a legal scalser\n", scal);
                exit(0);
            }
        }

        if (reg1_len > 0)
        {
            od->reg1 = reflect_register(reg1, cr);
        }

        if (reg2_len > 0)
        {
            od->reg2 = reflect_register(reg2, cr);
        }

        // set operand type
        if (cb == 0)
        {
            if (imm_len > 0)
            {
                od->type = MEM_IMM_REG1;
                return;
            }
            else
            {
                od->type = MEM_REG1;
                return;
            }
        }
        else if (cb == 1)
        {
            if (imm_len > 0)
            {
                od->type = MEM_IMM_REG1_REG2;
                return;
            }
            else
            {
                od->type = MEM_REG1_REG2;
                return;
            }
        }
        else if (cb == 2)
        {
            if (reg1_len > 0)
            {
                if (imm_len > 0)
                {
                    od->type = MEM_IMM_REG1_REG2_SCAL;
                    return;
                }
                else
                {
                    od->type = MEM_REG1_REG2_SCAL;
                    return;
                }
            }
            else
            {
                if (imm_len > 0)
                {
                    od->type = MEM_IMM_REG1_REG2_SCAL;
                    return;
                }
                else
                {
                    od->type = MEM_REG2_SCAL;
                    return;
                }
            }
        }
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
    cr->flags.__cpu_flag_value = 0;
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

/// @brief print current register info
void print_register(core_t *cr);

void print_register(core_t *cr)
{
    printf("rax = %16lx\trbx = %16lx\trcx = %16lx\trdx = %16lx\n",
           reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = %16lx\trdi = %16lx\trbp = %16lx\trsp = %16lx\n",
           reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = %16lx\n", reg.rip);

    printf("CF = %u\tZF = %u\tSF = %u\tOF = %u\t",
           cr->flags.CF, cr->flags.ZF, cr->flags.SF, cr->flags.OF);
    return;
}

/// @brief print current stack info
void print_stack(core_t *cr);

void print_stack(core_t *cr)
{
    if ((DEBUG_VERBOSE_SET & DEBUG_PRINTSTACK) == 0x0)
    {
        return;
    }

    int curr = 10;
    uint64_t *high = (uint64_t *)&mm[va2pa((cr->reg).rsp, cr)];
    high = &high[curr];

    uint64_t va = (cr->reg).rsp + curr * 8;

    for (int i = 0; i < curr * 2; ++i)
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%16lx : %16lx", va, (uint64_t)*prt);

        if (i == curr)
        {
            printf(" <== rsp");
        }

        va -= 8;

        printf("\n");
    }
    return;
}
