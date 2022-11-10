#ifndef LINKER_H
#define LINKER_H

#include <stdint.h>
#include <stdlib.h>

#define MAX_CHAR_SECTION_NAME (32)
#define MAX_CHAR_SYMBAl_NAME (64)


typedef enum E_ST_BIND
{
    STB_LOCAL,
    STB_GLOBAL,
    STB_WEAK
} st_bind_t;

typedef enum E_ST_TYPE
{
    STT_NOTYPE,
    STT_OBJECT,
    STT_FUNC
} st_type_t;

// section header table entry
typedef struct SHT_ENTRY
{
    char sh_name[MAX_CHAR_SECTION_NAME];
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
} sh_entry_t;

// symbal table
typedef struct 
{
    char st_name[MAX_CHAR_SYMBAl_NAME];
    st_bind_t bind;
    st_type_t type;
    char st_shndx[MAX_CHAR_SECTION_NAME];
    uint64_t st_value;
    uint64_t st_size;

} st_entry_t;

#define MAX_ELF_FILE_LENGTH (64)
#define MAX_ELF_FILE_WIDTH  (128)

typedef struct ELF_STRUCT
{
    char buffer[MAX_ELF_FILE_LENGTH][MAX_ELF_FILE_WIDTH];
    uint64_t line_count;

    uint64_t sht_count;
    sh_entry_t *sht;

    uint64_t symtab_count;
    st_entry_t *symtab;
} elf_t;

void parse_elf(char *filename, elf_t *elf);
void free_elf(elf_t *elf);

#endif