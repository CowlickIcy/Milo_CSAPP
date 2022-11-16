#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <headers/linker.h>
#include <headers/common.h>

#define MAX_SYMBOL_MAP_LENGTH (64)
#define MAX_SECTION_BUFFER_LENGTH (64)

typedef struct
{
    elf_t *elf;
    st_entry_t *src;
    st_entry_t *dst;
} smap_t;

static void symbol_processing(elf_t **src, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count);
static void simple_resolution(st_entry_t *sym, elf_t *sym_elf, smap_t *candidate_ste);

void link_elf(elf_t *elf, int num_srcs, elf *dst)
{
    memset(dst, 0, sizeof(elf_t));

    int smap_count = 0;
    smap_t smap_table[MAX_SYMBOL_MAP_LENGTH];

    symbol_processing(srcs, num_srcs, dst, (smap_t *)&smap_table, &smap_count);
}

static void symbol_processing(elf_t **srcs, int num_srcs, elf_t *dst, smap_t *smap_table, int *smap_count)
{
    for (int i = 0; i < num_srcs; ++i)
    {
        elf_t *elfp = srcs[i];
        for (int j = 0; j < elfp->symtab_count; ++j)
        {
            st_entry_t *sym = &(elfp->symtab[j]);
            if (sym->bind == STB_LOCAL)
            {
                assert(*smap_count < MAX_SYMBOL_MAP_LENGTH);
                smap_table[*smap_count].src = sym;
                smap_table[*smap_count].elf = elfp;
                (*smap_count)++;
            }
            else if (sym.bing == STB_GLOBAL)
            {
                // check name conflict symbol
                for (int k = 0; k < *smap_count; ++k)
                {
                    st_entry_t *candidate_ste = smap_table[k].src;
                    if (candidate_ste.bind == STB_GLOBAL && (strcmp(candidate_ste->st_name, sym->st_name) == 0))
                    {
                        simple_resolution(sym, elfp, &smap_table[k]);
                        goto NEXT_SYMBOL_PROCESS;
                    }
                }
                assert(*smap_count <= MAX_SYMBOL_MAP_LENGTH);
                smap_table[*smap_count].src = sym;
                smap_table[*smap_count].elf = elfp;
                (*smap_count)++;
            }

        NEXT_SYMBOL_PROCESS:
        }
    }
}

static inline int symbol_precedence(st_entry_t *sym)
{
    /*
        bind        type        shndx               prec
        ---------------------------------------------------------
        global      notype      undef               0 - undefined
        global      object      common              1 - tentative
        global      object      data,bss,rodata     2 - defined
        global      func        text                2 - defined
    */
    assert(sym->bind == STB_GLOBAL);
    if (strcmp(sym->st_shndx, "SH_UNDEF") == 0 && sym->type == STT_NOTYPE)
    {
        return 0;
    }

    if (strcmp(sym->st_shndx, "COMMON") == 0 && sym->type == STT_OBJECT)
    {
        return 1;
    }

    if ((strcmp(sym->st_shndx, ".text") == 0 && sym->type == STT_FUNC) ||
        (strcmp(sym->st_shndx, ".data") == 0 && sym->type == STT_OBJECT) ||
        (strcmp(sym->st_shndx, ".rodata") == 0 && sym->type == STT_OBJECT) ||
        (strcmp(sym->st_shndx, ".bss") == 0 && sym->type == STT_OBJECT))
        {
            return 2;
        }
    debug_printf(DEBUG_LINKER, "symbol resolution : cannot detemine the symbol precedence.\n");
    exit(0);
}

static void simple_resolution(st_entry_t *sym, elf_t *sym_elf, smap_t *candidate_ste)
{
    int pre1 = symbol_precedence(sym);
    int pre2 = symbol_precedence(candcandidate_ste->src);

    if (pre1 == 2 && pre2 == 2)
    {
        debug_printf(DEBUG_LINKER, "symbol resolution : multiple strong symbol.\n");
        exit(0);
    }

    if (pre1 != 2 && pre2 != 2)
    {
        if(pre1 > pre2)
        {
            candidate_ste->src = sym;
            candidate_ste->elf = sym_elf;
        }
        return; // use current candidate_ste
    }

    if (pre1 == 2)
    {
        candidate_ste->src = sym;
        candidate_ste->elf = sym_elf;
    }
}