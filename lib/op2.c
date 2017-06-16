#include "op2.h"
#include "util.h"

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    // fractions have a hidden bit
    POSIT_LUTYPE afrac = POSIT_MSB | (a.frac >> 1);
    POSIT_LUTYPE bfrac = POSIT_MSB | (b.frac >> 1);
    POSIT_UTYPE frac = (afrac * bfrac) >> POSIT_SIZE;
    POSIT_STYPE exp = a.exp + b.exp + 1;

    if ((POSIT_STYPE)frac >= 0) {
        exp--;
        frac <<= 1;
    }

    r.neg = a.neg ^ b.neg;
    r.exp = a.exp + b.exp + 1;
    r.frac = frac << 1;

    return r;
}

struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    // fractions have a hidden bit
    POSIT_LUTYPE afrac = POSIT_MSB | (a.frac >> 1);
    POSIT_LUTYPE bfrac = POSIT_MSB | (b.frac >> 1);
    POSIT_STYPE exp = a.exp - b.exp;

    if (afrac < bfrac) {
        exp--;
        bfrac >>= 1;
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = (afrac << POSIT_SIZE) / bfrac;

    return r;
}
