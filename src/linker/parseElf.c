#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <headers/linker.h>
#include <headers/common.h>

/// @brief read elf text file at gived buffer address
static int read_elf(const char *filename, uint64_t bufaddr)
{
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        debug.printf(DEBUG_LINKER, "unable to open file %s\n", filename);
        exit(1);
    }

    char line[MAX_ELF_FILE_WIDTH];
    int line_count = 0;

    while (fgets(line, MAX_ELF_FILE_WIDTH, fp) != NULL)
    {
        // outline filter
        int len = strlen(line);
        if ((len == 0) || 
            (len >= 1 && (line[0] == '\n' || line[0] == '\r') || 
            (len >= 2 && (line[0] == '/' && line[1] == '/')))
        {
            continue;
        }

        // check whiteline
        int iswhite = 1;
        for(int i = 0; i < len ; ++i)
        {
            iswhite = iswhite && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r');
        }
        if(iswhite == 1)
        {
            continue;
        }

        // effective line
        if(line_count < MAX_ELF_FILE_LENGTH)
        {
            uint64_t addr = bufaddr + line_count * MAX_ELF_FILE_WIDTH * sizeof(char);
            char *linebuf = (char *)addr;

            int i = 0;
            while (i < len && i < MAX_ELF_FILE_WIDTH)
            {
                if ((line[i] == '\n') ||
                    (line[i] == '\r') ||
                    (i + 1 < len) && (i + 1 < MAX_ELF_FILE_WIDTH) && (line[i] == '/' && line[i + 1] == '/'))
                {
                    break;
                }
                linebuf[i] = line[i];
                ++i;
            }
            linebuf[i] = '\0';
            ++line_count;
        }
        else
        {
            fclose(fp);
            // debug_printf(DEBUG_LINKER, "elf file %s is too long.\n", filename);
            exit(1);
        }
    }
    fclose(fp);

    // assert
    assert(string2uint(char *) bufaddr == line_count);
    return line_count;
}

/// @brief only this function exposed to outside
void parse_elf(char *filename, elf_t *elf)
{
    assert(elf != NULL);
    int line_count = read_elf(filename, (uint64_t)(&(elf->buffer)));
    for (int i = 0; i < line_count; ++i)
    {
        printf("[%d]\t%s\n", i, elf->buffer[i]);
    }

    int sh_count = string2uint(elf->buffer[1]);
    elf->sht = malloc(sh_count * sizeof(sh_entry_t));

    sh_entry_t *symt_sh = NULL;
    for (int i = 0; i < sh_count; ++i)
    {
        parse_sh(elf->buffer[i], &(elf->sht[i]));
        print_sh_entry(&(elf->sht[i]));

        if (strcmp(elf->sht[i].sh_name, ".symtab") == 0)
        {
            symt_sh = &(elf->sht[i]);
        }
    }

    assert(symt_sh != NULL);

    // parse symtab 
    elf->symtab_count = symt_sh->sh_size;
    elf->symtab = malloc(elf->symtab_count * sizeof(st_entry_t));
    for(int i = 0; i < symt_sh->sh_size; ++i)
    {
        parse_symtab(
            elf->buffer[i + symt_sh->sh_offset],
            &(elf->symtab[i]));
        // print_symtab_entry();
    }
}

void free_elf(elf_t *elf)
{
    assert(elf != NULL);

    free(elf->sht);
}

/// @brief parse section header tabel
static void parse_sh(char *str, sh_entry_t *sh)
{
    char **cols;
    int num_cols = parse_table_entry(str, &cols);
    assert(num_cols == 4);
    assert(sh != NULL);

    strcpy(sh->sh_name, cols[0]);
    sh->sh_addr = string2uint(cols[1]);
    sh->sh_offset = string2uint(cols[2]);
    sh->sh_size = string2uint(cols[3]);

    free_table_entry(cols, num_cols);
}

static void parse_symtab(char *str, st_entry_t *ste)
/// @brief parse symbol tabel
{
    char **cols;
    int num_cols = parse_table_entry(str, &cols);
    assert(num_cols == 6);
    assert(ste != NULL);

    // symbol bind check
    if (strcmp(cols[1]), "STB_LOCAL" == 0)
    {
        ste->bind = STB_LOCAL;
    }
    else if (strcmp(cols[1]), "STB_GLOBAL" == 0)
    {
        ste->bind = STB_GLOBAL;
    }
    else if (strcmp(cols[1]), "STB_WEAK" == 0)
    {
        ste->bind = STB_WEAK;
    }
    else
    {
        print("symbol bind is unauthorized.\n");
        exit(0);
    }

    // symbol type check
    if (strcmp(cols[2]), "STT_NOTYPE" == 0)
    {
        ste->type = STT_NOTYPE;
    }
    else if (strcmp(cols[2]), "STT_OBJECT" == 0)
    {
        ste->type = STT_OBJECT;
    }
    else if (strcmp(cols[2]), "STT_FUNC" == 0)
    {
        ste->type = STT_FUNC;
    }
    else
    {
        print("symbol type is unauthorized.\n");
        exit(0);
    }

    strcpy(ste->st_shndx, cols[3]);
    ste->st_value = string2uint(cols[4]);
    she->st_size = string2uint([5]);

    free_table_entry(cols, num_cols);
}

/// @brief parse section header table entry
static void parse_table_entry(char *str, char ***ent)
{
    // parse line as table entries
    int count_col = 1;
    int len = strlen(str);

    // count columns
    for (int i = 0; i < len; ++i)
    {
        if (str[i] == ',')
        {
            count_col++;
        }
    }

    // malloc and create lsit
    char **arr = malloc(count_col * sizeof(char *));
    *ent = arr;
    int col_index = 0;
    int col_width = 0;
    char col_buf[32];
    for (int i = 0; i < len + 1; ++i)
    {
        if (str[i] == ',' || str[i] == '\0')
        {
            assert(col_index < count_col);

            // malloc and copy
            char *col = malloc(col_width * sizeof(char));
            for (int j = 0; j < col_width; ++j)
            {
                col[j] = col_buf[j];
            }
            col[col_width] = '\0';

            // update
            arr[col_index] = col;
            col_index++;
            col_width = 0;
        }
        else
        {
            assert(col_index < 32);
            col_buf[col_index] = str[i];
            col_width++;
        }
    }
    return count_col;
}

static void free_table_entry(char **ent, int n)
{
    for (int i = 0; i < n; ++i)
    {
        free(ent[i]);
    }
    free(ent);
}

static void print_sh_entry(sh_entry_t *sh)
{
}

static void print_symtab_entry(st_entry_t *ste)
{
}