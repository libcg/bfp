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

float pack_float(struct unpkd_posit_t up, int es)
{
    int fexp = POW2(es) * up.reg + up.exp + 127;

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

double pack_double(struct unpkd_posit_t up, int es)
{
    int fexp = POW2(es) * up.reg + up.exp + 1023;

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

struct unpkd_posit_t unpack_posit(POSIT_UTYPE p, int nbits, int es)
{
    struct unpkd_posit_t up;

    up.neg = util_is_neg(p);
    if (up.neg) {
        p = util_neg(p, nbits);
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
