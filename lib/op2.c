#include "op2.h"
#include "util.h"

static struct unpacked_t add(struct unpacked_t a, struct unpacked_t b, bool neg)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_LUTYPE frac;

    if (a.exp > b.exp) {
        r.exp = a.exp;
        bfrac >>= a.exp - b.exp;
    } else {
        r.exp = b.exp;
        afrac >>= b.exp - a.exp;
    }

    frac = afrac + bfrac;
    if (frac >> POSIT_WIDTH != 0) {
        r.exp++;
        frac >>= 1;
    }

    r.neg = neg;
    r.frac = frac << 1;

    return r;
}

static struct unpacked_t sub(struct unpacked_t a, struct unpacked_t b, bool neg)
{
    struct unpacked_t r;

    POSIT_UTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_UTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_UTYPE frac;

    if (a.exp > b.exp || (a.exp == b.exp && a.frac > b.frac)) {
        r.exp = a.exp;
        bfrac >>= a.exp - b.exp;
        frac = afrac - bfrac;
    } else {
        neg = !neg;
        r.exp = b.exp;
        afrac >>= b.exp - a.exp;
        frac = bfrac - afrac;
    }

    r.neg = neg;
    r.exp -= CLZ(frac);
    r.frac = frac << (CLZ(frac) + 1);

    return r;
}

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_UTYPE frac = (afrac * bfrac) >> POSIT_WIDTH;
    POSIT_STYPE exp = a.exp + b.exp + 1;

    if ((frac & POSIT_MSB) == 0) {
        exp--;
        frac <<= 1;
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = frac << 1;

    return r;
}

struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b)
{
    struct unpacked_t r;

    POSIT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    POSIT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    POSIT_STYPE exp = a.exp - b.exp;

    if (afrac < bfrac) {
        exp--;
        bfrac >>= 1;
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = (afrac << POSIT_WIDTH) / bfrac;

    return r;
}

struct unpacked_t op2_add(struct unpacked_t a, struct unpacked_t b)
{
    if (a.neg == b.neg) {
        return add(a, b, a.neg);
    } else {
        return sub(a, b, a.neg);
    }
}

struct unpacked_t op2_sub(struct unpacked_t a, struct unpacked_t b)
{
    if (a.neg == b.neg) {
        return sub(a, b, a.neg);
    } else {
        return add(a, b, a.neg);
    }
}
