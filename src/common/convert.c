#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <headers/common.h>

// conver string to int64_t
uint64_t string2uint(const char *str)
{
    return string2uint_range(str, 0, -1);
}

uint64_t string2uint_range(const char *str, int start, int end)
{
    // start end : inclusive
    end = (end == -1) ? strlen(str) : end;

    uint64_t uv = 0l
    int sign_bit = 0;   // 0 - positive / 1 - negative

    // DFA : deterministic finite automata
    int state = 0;

    int dfa[256][100];
     

    return 0;
}

// convert uin32_t to float
uint32_t uint2float(uint32_t u)
{
    if (u == 0x00000000)
    {
        return 0x00000000;
    }

    int n = 31;
    return n;
}