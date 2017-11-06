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
    int ss = util_ss();
    int rs = MAX(-reg + 1, reg + 2);

    // FIXME: round exponent up if needed
    if (ss + rs + es >= nbits && up.frac >= POSIT_MSB) {
        up.exp++;
        reg = FLOORDIV(up.exp, POW2(es));
        rs = MAX(-reg + 1, reg + 2);
    }

    POSIT_UTYPE exp = up.exp - POW2(es) * reg;

    if (reg < 0) {
        regbits = POSIT_MSB >> -reg;
    } else {
        regbits = LMASK(POSIT_MASK, reg + 1);
    }
    expbits = LMASK(exp << (POSIT_WIDTH - es), es);

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
    int fexp = up.exp + 127;

    // left aligned
    uint32_t fexpbits;
    uint32_t ffracbits;

    if (fexp > 254) {
        // overflow, set maximum value
        fexpbits = 254 << 24;
        ffracbits = -1;
    } else if (fexp < 1) {
        // underflow, pack as denormal
        fexpbits = 0;
#if POSIT_WIDTH <= 32
        ffracbits = (uint32_t)(POSIT_MSB | (up.frac >> 1)) << (32 - POSIT_WIDTH);
#else
        ffracbits = (POSIT_MSB | (up.frac >> 1)) >> (POSIT_WIDTH - 32);
#endif
        ffracbits >>= -fexp;
    } else {
        fexpbits = (fexp & 0xFF) << 24;
#if POSIT_WIDTH <= 32
        ffracbits = (uint32_t)up.frac << (32 - POSIT_WIDTH);
#else
        ffracbits = up.frac >> (POSIT_WIDTH - 32);
#endif
    }

    union {
        float f;
        uint32_t u;
    } un;

    un.u = ffracbits;
    un.u = fexpbits | (un.u >> 8);
    un.u = (up.neg << 31) | (un.u >> 1);

    // don't underflow to zero
    if ((un.u << 1) == 0) {
        un.u++;
    }

    return un.f;
}

double pack_double(struct unpacked_t up)
{
    int fexp = up.exp + 1023;

    // left aligned
    uint64_t fexpbits;
    uint64_t ffracbits;

    if (fexp > 2046) {
        // overflow, set maximum value
        fexpbits = (uint64_t)2046 << 53;
        ffracbits = -1;
    } else if (fexp < 1) {
        // underflow, pack as denormal
        fexpbits = 0;
#if POSIT_WIDTH <= 64
        ffracbits = (uint64_t)(POSIT_MSB | (up.frac >> 1)) << (64 - POSIT_WIDTH);
#else
        ffracbits = (POSIT_MSB | (up.frac >> 1)) >> (POSIT_WIDTH - 64);
#endif
        ffracbits >>= -fexp;
    } else {
        fexpbits = (uint64_t)(fexp & 0x7FF) << 53;
#if POSIT_WIDTH <= 64
        ffracbits = (uint64_t)up.frac << (64 - POSIT_WIDTH);
#else
        ffracbits = up.frac >> (POSIT_WIDTH - 64);
#endif
    }

    union {
        double f;
        uint64_t u;
    } un;

    un.u = ffracbits;
    un.u = fexpbits | (un.u >> 11);
    un.u = ((uint64_t)up.neg << 63) | (un.u >> 1);

    // don't underflow to zero
    if ((un.u << 1) == 0) {
        un.u++;
    }

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
    POSIT_UTYPE exp = (p << (ss + rs)) >> (POSIT_WIDTH - es);

    up.neg = neg;
    up.exp = POW2(es) * reg + exp;
    up.frac = p << (ss + rs + es);

    return up;
}

struct unpacked_t unpack_float(float f)
{
    struct unpacked_t up;
    int bias = 127;

    union {
        float f;
        uint32_t u;
    } un;

    un.f = f;

    up.neg = un.u >> 31;
    up.exp = ((un.u >> 23) & 0xFF) - bias;
#if POSIT_WIDTH <= 32
    up.frac = (un.u << 9) >> (32 - POSIT_WIDTH);
#else
    up.frac = (POSIT_UTYPE)un.u << (POSIT_WIDTH - 32 + 9);
#endif

    if (up.exp == -bias) {
        // normalize
        // FIXME: some precision is lost if frac was downcasted
        up.exp -= CLZ(up.frac);
        up.frac <<= CLZ(up.frac) + 1;
    }

    return up;
}

struct unpacked_t unpack_double(double f)
{
    struct unpacked_t up;
    int bias = 1023;

    union {
        double f;
        uint64_t u;
    } un;

    un.f = f;

    up.neg = un.u >> 63;
    up.exp = ((un.u >> 52) & 0x7FF) - bias;
#if POSIT_WIDTH <= 64
    up.frac = (un.u << 12) >> (64 - POSIT_WIDTH);
#else
    up.frac = (POSIT_UTYPE)un.u << (POSIT_WIDTH - 64 + 12);
#endif

    if (up.exp == -bias) {
        // normalize
        // FIXME: some precision is lost if frac was downcasted
        up.exp -= CLZ(up.frac);
        up.frac <<= CLZ(up.frac) + 1;
    }

    return up;
}
