#include "pack.h"
#include "util.h"

POSIT_UTYPE pack_posit(struct unpkd_posit_t p, int nbits, int es)
{
    POSIT_UTYPE bits;
    POSIT_UTYPE regBits;
    POSIT_UTYPE expBits;
    int ss = util_ss();
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

struct unpkd_posit_t unpack_posit(POSIT_UTYPE p, int nbits, int es)
{
    struct unpkd_posit_t up;

    if (util_is_neg(p)) {
        up.neg = true;
        p = util_neg(p, nbits);
    } else {
        up.neg = false;
    }

    int ss = util_ss();
    int rs = util_rs(p, nbits);

    int lz = CLZ(p << ss);
    int lo = CLZ(~p << ss);

    up.reg = (lz == 0 ? lo - 1 : -lz);
    up.exp = (p << (ss + rs)) >> (POSIT_SIZE - es);
    up.frac = p << (ss + rs + es);

    return up;
}
