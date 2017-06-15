#include "op2.h"
#include "util.h"

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    // fractions have a hidden bit
    POSIT_LUTYPE afrac = POSIT_MSB | (a.frac >> 1);
    POSIT_LUTYPE bfrac = POSIT_MSB | (b.frac >> 1);
    POSIT_UTYPE frac = (afrac * bfrac) >> POSIT_SIZE;

    // shift is either 0 or 1
    int shift = CLZ(frac);

    r.neg = a.neg ^ b.neg;
    r.exp = a.exp + b.exp - shift + 1;
    r.frac = frac << (shift + 1);

    return r;
}
