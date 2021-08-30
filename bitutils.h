#ifndef BITUTILS_H_
#define BITUTILS_H_

#include <stdint.h>

#define LOG2(X) ((unsigned) (8*sizeof (uint64_t) - __builtin_clzll((X)) - 1))

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

int pop_next_index(uint64_t* in);
inline int pop_next_index(uint64_t* in)
{
    int ret = LOG2(*in);
    *in = set_nth_bit_to(*in, ret, 0);
    return ret;
}
#endif // BITUTILS_H_
