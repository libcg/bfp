#include "pack.h"
#include "util.h"

POSIT_UTYPE pack_posit(struct unpacked_t up, int nbits, int es)
{
    POSIT_UTYPE p;
    POSIT_UTYPE regbits;
    POSIT_UTYPE expbits;

    // handle underflow and overflow.
    // in either case, exponent and fraction bits will disappear.
    int maxexp = POW2(es) * (nbits - 2);
    if (up.exp < -maxexp) {
        up.exp = -maxexp;
    } else if (up.exp > maxexp) {
        up.exp = maxexp;
    }

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

float pack_float(struct unpacked_t up)
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
    } un;

    un.u = ffracbits;
    un.u = fexpbits | (un.u >> 8);
    un.u = (up.neg << 31) | (un.u >> 1);

    return un.f;
}

double pack_double(struct unpacked_t up)
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
    } un;

    un.u = ffracbits;
    un.u = fexpbits | (un.u >> 11);
    un.u = ((uint64_t)up.neg << 63) | (un.u >> 1);

    return un.f;
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

struct unpacked_t unpack_float(float f)
{
    struct unpacked_t up;

    union {
        float f;
        uint32_t u;
    } un;

    un.f = f;

    up.neg = un.u >> 31;
    up.exp = ((un.u >> 23) & 0xFF) - 127;
#if POSIT_SIZE <= 32
    up.frac = (un.u << 9) >> (32 - POSIT_SIZE);
#else
    up.frac = (POSIT_UTYPE)un.u << (POSIT_SIZE - 32 + 9);
#endif

    return up;
}

struct unpacked_t unpack_double(double f)
{
    struct unpacked_t up;

    union {
        double f;
        uint64_t u;
    } un;

    un.f = f;

    up.neg = un.u >> 63;
    up.exp = ((un.u >> 52) & 0x7FF) - 1023;
#if POSIT_SIZE <= 64
    up.frac = (un.u << 12) >> (64 - POSIT_SIZE);
#else
    up.frac = (POSIT_UTYPE)un.u << (POSIT_SIZE - 64 + 12);
#endif

    return up;
}
