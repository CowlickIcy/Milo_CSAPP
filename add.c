#include <stdint.h>

uint64_t add(uint64_t a, uint64_t b)
{
    return a + b;
}
int main()
{
    uint64_t a = 0x12340000;
    uint64_t b = 0x0000abcd;
    a = add(a, b);
    return 0;
}