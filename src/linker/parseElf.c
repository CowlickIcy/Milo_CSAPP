#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <headers/linker.h>
#include <headers/common.h>

// read elf
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

        // inline filter


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
            exit(0);
        }
    }
    fclose(fp);

    // assert
    assert(string2uint(char *)bufaddr == line_count);
    return line_count;
}