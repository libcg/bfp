#include "util.h"

bool util_is_zero(POSIT_UTYPE p)
{
    return p == POSIT_ZERO;
}

bool util_is_nar(POSIT_UTYPE p)
{
    return p == POSIT_NAR;
}

bool util_is_neg(POSIT_UTYPE p)
{
    return (POSIT_STYPE)p < 0 && !util_is_nar(p);
}

int util_ss()
{
    return 1;
}

int util_rs(POSIT_UTYPE p, int nbits)
{
    int ss = util_ss();
    int lz = CLZ(LSHIFT(p, ss));
    int lo = CLZ(LSHIFT(~p, ss));
    int rs = MAX(lz, lo) + 1;

    return MIN(rs, nbits - ss);
}

int util_es(POSIT_UTYPE p, int nbits, int es)
{
    int ss = util_ss();
    int rs = util_rs(p, nbits);

    return MIN(MAX(nbits - ss - rs, 0), es);
}

int util_fs(POSIT_UTYPE p, int nbits, int es)
{
    int ss = util_ss();
    int rs = util_rs(p, nbits);

    return MAX(nbits - ss - rs - es, 0);
}

POSIT_UTYPE util_neg(POSIT_UTYPE p, int nbits)
{
    // reverse all bits and add one
    return LMASK(-LMASK(p, nbits), nbits);
}
