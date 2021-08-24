#ifndef BITUTILS_H_
#define BITUTILS_H_

#include <stdint.h>

inline uint64_t set_nth_bit_to(uint64_t integer, int n, int val)
{
    integer ^= (-val ^ integer) & (1ULL << n);
    return integer;
}

uint64_t set_nth_bit_to(uint64_t integer, int n, int val);

inline int get_nth_bit(uint64_t integer, int n)
{
    return (integer >> n) & 1U;
}
int get_nth_bit(uint64_t integer, int n);

inline int popcount(uint64_t in)
{
    int ret;

    for (ret = 0; in; ret++)
    {
      in &= in - 1; // clear the least significant bit set
    }
    return ret;
}

int popcount(uint64_t in);

#endif // BITUTILS_H_
