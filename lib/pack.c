#include "pack.h"
#include "util.h"

POSIT_UTYPE pack_posit(struct unpkd_posit_t p, int nbits, int es)
{
    POSIT_UTYPE bits;
    POSIT_UTYPE regBits;
    POSIT_UTYPE expBits;
    int ss = 1;
    int rs = MAX(-p.reg + 1, p.reg + 2);

    if (p.reg < 0) {
        regBits = POSIT_MSB >> -p.reg;
    } else {
        regBits = LMASK(POSIT_MASK, p.reg + 1);
    }
    expBits = LMASK(p.exp << (POSIT_SIZE - es), es);

    bits = p.frac;
    bits = expBits | (bits >> es);
    bits = regBits | (bits >> rs);
    bits = bits >> ss;

    if (p.neg) {
        bits = (bits ^ POSIT_MASK) + 1;
    }

    return LMASK(bits, nbits);
}
