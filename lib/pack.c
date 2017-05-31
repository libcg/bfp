#include "pack.h"
#include "util.h"

POSIT_UTYPE pack_posit(struct unpkd_posit_t up, int nbits, int es)
{
    POSIT_UTYPE p;
    POSIT_UTYPE regBits;
    POSIT_UTYPE expBits;

    int ss = util_ss();
    int rs = MAX(-up.reg + 1, up.reg + 2);

    if (up.reg < 0) {
        regBits = POSIT_MSB >> -up.reg;
    } else {
        regBits = LMASK(POSIT_MASK, up.reg + 1);
    }
    expBits = LMASK(up.exp << (POSIT_SIZE - es), es);

    p = up.frac;
    p = expBits | (p >> es);
    p = regBits | (p >> rs);
    p = p >> ss;

    if (up.neg) {
        return util_neg(p, nbits);
    } else {
        return LMASK(p, nbits);
    }
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
