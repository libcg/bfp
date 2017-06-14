#include "pack.h"
#include "util.h"

POSIT_UTYPE pack_posit(struct unpacked_t up, int nbits, int es)
{
    POSIT_UTYPE p;
    POSIT_UTYPE regbits;
    POSIT_UTYPE expbits;

    int reg = FLOORDIV(up.exp, POW2(es));
    POSIT_UTYPE exp = up.exp - POW2(es) * reg;

    int ss = util_ss();
    int rs = MAX(-reg + 1, reg + 2);

    if (reg < 0) {
        regbits = POSIT_MSB >> -reg;
    } else {
        regbits = LMASK(POSIT_MASK, reg + 1);
    }
    expbits = LMASK(exp << (POSIT_SIZE - es), es);

    p = up.frac;
    p = expbits | (p >> es);
    p = regbits | (p >> rs);
    p = p >> ss;

    if (up.neg) {
        return util_neg(p, nbits);
    } else {
        return LMASK(p, nbits);
    }
}

float pack_float(struct unpacked_t up, int es)
{
    int fexp = MIN(MAX(up.exp + 127, 0), 255);

    // left aligned
    uint32_t fexpbits;
    uint32_t ffracbits;

    if (fexp < 1) {
        // TODO: support subnormals (exponent 0)
        // underflow, set minimum value
        fexpbits = 1 << 24;
        ffracbits = 0;
    } else if (fexp > 254) {
        // overflow, set maximum value
        fexpbits = 254 << 24;
        ffracbits = -1;
    } else {
        fexpbits = (fexp & 0xFF) << 24;
#if POSIT_SIZE <= 32
        ffracbits = (uint32_t)up.frac << (32 - POSIT_SIZE);
#else
        ffracbits = up.frac >> (POSIT_SIZE - 32);
#endif
    }

    union {
        float f;
        uint32_t u;
    } f;

    f.u = ffracbits;
    f.u = fexpbits | (f.u >> 8);
    f.u = (up.neg << 31) | (f.u >> 1);

    return f.f;
}

double pack_double(struct unpacked_t up, int es)
{
    int fexp = MIN(MAX(up.exp + 1023, 0), 2047);

    // left aligned
    uint64_t fexpbits;
    uint64_t ffracbits;

    if (fexp < 1) {
        // TODO: support subnormals (exponent 0)
        // underflow, set minimum value
        fexpbits = (uint64_t)1 << 53;
        ffracbits = 0;
    } else if (fexp > 2046) {
        // overflow, set maximum value
        fexpbits = (uint64_t)2046 << 53;
        ffracbits = -1;
    } else {
        fexpbits = (uint64_t)(fexp & 0x7FF) << 53;
#if POSIT_SIZE <= 64
        ffracbits = (uint64_t)up.frac << (64 - POSIT_SIZE);
#else
        ffracbits = up.frac >> (POSIT_SIZE - 64);
#endif
    }

    union {
        double f;
        uint64_t u;
    } f;

    f.u = ffracbits;
    f.u = fexpbits | (f.u >> 11);
    f.u = ((uint64_t)up.neg << 63) | (f.u >> 1);

    return f.f;
}

struct unpacked_t unpack_posit(POSIT_UTYPE p, int nbits, int es)
{
    struct unpacked_t up;

    bool neg = util_is_neg(p);
    if (neg) {
        p = util_neg(p, nbits);
    }

    int ss = util_ss();
    int rs = util_rs(p, nbits);

    int lz = CLZ(p << ss);
    int lo = CLZ(~p << ss);

    int reg = (lz == 0 ? lo - 1 : -lz);
    POSIT_UTYPE exp = (p << (ss + rs)) >> (POSIT_SIZE - es);

    up.neg = neg;
    up.exp = POW2(es) * reg + exp;
    up.frac = p << (ss + rs + es);

    return up;
}
