#include "posit.h"

#include <cstdio>
#include <cmath>

using namespace std;

#ifdef __GNUC__
#define CLZ(n) \
    __builtin_clz(n)
#endif

#define POW2(n) \
    (1 << (n))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

POSIT_UTYPE Posit::buildMask(unsigned size)
{
    return POSIT_MASK << (POSIT_SIZE - size);
}

POSIT_UTYPE Posit::buildBits(bool neg, signed reg, POSIT_UTYPE exp,
                             POSIT_UTYPE frac)
{
    POSIT_UTYPE bits;
    POSIT_UTYPE regBits;
    POSIT_UTYPE expBits;
    unsigned rs = MAX(-reg + 1, reg + 2);

    if (reg < 0) {
        regBits = POSIT_MSB >> (-reg);
    } else {
        regBits = buildMask(reg + 1);
    }
    expBits = (exp << (POSIT_SIZE - mEs)) & buildMask(mEs);

    bits = frac;
    bits = expBits | (bits >> mEs);
    bits = regBits | (bits >> rs);
    bits = bits >> ss();

    if (neg) {
        bits = (bits ^ POSIT_MASK) + 1;
    }

    return bits & buildMask(mNbits);
}

POSIT_UTYPE Posit::ieeeToBits(uint64_t fbits, unsigned fes, unsigned ffs)
{
    int16_t fexp = (fbits >> ffs) & ((1 << fes) - 1);
    uint64_t ffrac = fbits & ((1LL << ffs) - 1);

    // clip exponent
    signed rminfexp = POW2(mEs) * (-mNbits + 2);
    signed rmaxfexp = POW2(mEs) * (mNbits - 2);
    signed rfexp = MIN(MAX(fexp - POW2(fes - 1) + 1, rminfexp), rmaxfexp);

    bool rsign = fbits >> (fes + ffs);
    signed rreg = rfexp >> mEs; // floor(rfexp / 2^mEs)
    unsigned rexp = rfexp - POW2(mEs) * rreg;
    POSIT_UTYPE rfrac;

    if (ffs <= POSIT_SIZE) {
        rfrac = ffrac << (POSIT_SIZE - ffs);
    } else {
        rfrac = ffrac >> (ffs - POSIT_SIZE);
    }

    return buildBits(rsign, rreg, rexp, rfrac);
}

Posit::Posit(POSIT_UTYPE bits, unsigned nbits, unsigned es, bool nan) :
    mBits(bits),
    mNbits(nbits),
    mEs(es),
    mNan(nan)
{
}

Posit::Posit(unsigned nbits, unsigned es, bool nan) :
    Posit(0, nbits, es, nan)
{
}

Posit::Posit(unsigned nbits, unsigned es) :
    Posit(nbits, es, false)
{
}

bool Posit::isZero()
{
    return mBits == POSIT_ZERO;
}

bool Posit::isOne()
{
    return mBits == POSIT_ONE || mBits == POSIT_MONE;
}

bool Posit::isInf()
{
    return mBits == POSIT_INF;
}

bool Posit::isNeg()
{
    return (POSIT_STYPE)mBits < 0 && mBits != POSIT_INF;
}

bool Posit::isNan()
{
    return mNan;
}

unsigned Posit::nbits()
{
    return mNbits;
}

unsigned Posit::ss()
{
    return 1;
}

unsigned Posit::rs()
{
    unsigned lz = CLZ(mBits << ss());
    unsigned lo = CLZ(~mBits << ss());
    unsigned rs = MAX(lz, lo) + 1;

    return MIN(rs, mNbits - ss());
}

unsigned Posit::es()
{
    return MIN(MAX(mNbits - ss() - rs(), 0), mEs);
}

unsigned Posit::fs()
{
    return MAX(mNbits - ss() - rs() - mEs, 0);
}

unsigned Posit::useed()
{
    return POW2(POW2(mEs));
}

signed Posit::regime()
{
    POSIT_UTYPE bits = isNeg() ? neg().mBits : mBits;
    unsigned lz = CLZ(bits << ss());
    unsigned lo = CLZ(~bits << ss());

    if (lz == 0)
        return lo - 1;
    else
        return -lz;
}

POSIT_UTYPE Posit::exponent()
{
    POSIT_UTYPE expBits = (mBits & (buildMask(mEs) >> (ss() + rs())));

    return expBits >> (POSIT_SIZE - mNbits + fs());
}

POSIT_UTYPE Posit::fraction()
{
    POSIT_UTYPE fracBits = (mBits & (buildMask(fs()) >> (ss() + rs() + mEs)));

    return fracBits >> (POSIT_SIZE - mNbits);
}

POSIT_UTYPE Posit::lfraction()
{
    // left-align bits
    return mBits << (ss() + rs() + mEs);
}

Posit Posit::zero()
{
    return Posit(POSIT_ZERO, mNbits, mEs, false);
}

Posit Posit::one()
{
    return Posit(POSIT_ONE, mNbits, mEs, false);
}

Posit Posit::inf()
{
    return Posit(POSIT_INF, mNbits, mEs, false);
}

Posit Posit::nan()
{
    return Posit(mNbits, mEs, true);
}

Posit Posit::neg()
{
    Posit p = Posit(mNbits, mEs);
    POSIT_UTYPE mask = buildMask(mNbits);

    // reverse all bits and add one
    p.mBits = ((mBits ^ POSIT_MASK) + 1) & mask;

    return p;
}

Posit Posit::rec()
{
    Posit p = Posit(mNbits, mEs);
    POSIT_UTYPE mask = buildMask(mNbits);

    // reverse all bits but the first one and add one
    p.mBits = ((mBits ^ (POSIT_MASK >> ss())) + 1) & mask;

    return p;
}

Posit Posit::add(Posit& p)
{
    // fast exit
    if (isZero()) {
        return p;
    } else if (p.isZero()) {
        return *this;
    } else if (isInf() && p.isInf()) {
        return nan();
    } else if (isInf() || p.isInf()) {
        return inf();
    } else if (neg().eq(p)) {
        return zero();
    }

    // TODO implement
    return *this;
}

Posit Posit::sub(Posit& p)
{
    // no loss on negation
    Posit np = p.neg();

    return add(np);
}

Posit Posit::mul(Posit& p)
{
    // fast exit
    if (isZero()) {
        return (p.isInf() ? nan() : zero());
    } else if (p.isZero()) {
        return (isInf() ? nan() : zero());
    } else if (isOne()) {
        return (isNeg() ? p.neg() : p);
    } else if (p.isOne()) {
        return (p.isNeg() ? neg() : *this);
    } else if (isInf() || p.isInf()) {
        return inf();
    } else if (rec().eq(p)) {
        return one();
    } else if (rec().neg().eq(p)) {
        return one().neg();
    }

    signed xfexp = POW2(mEs) * regime() + exponent();
    signed pfexp = POW2(mEs) * p.regime() + p.exponent();

    // fractions have a hidden bit
    POSIT_UTYPE xfrac = POSIT_MSB | (lfraction() >> 1);
    POSIT_UTYPE pfrac = POSIT_MSB | (p.lfraction() >> 1);
    POSIT_UTYPE mfrac = ((POSIT_LUTYPE)xfrac *
                         (POSIT_LUTYPE)pfrac) >> POSIT_SIZE;

    // shift is either 0 or 1
    signed shift = CLZ(mfrac);

    // clip exponent to avoid underflow and overflow
    signed rminfexp = POW2(mEs) * (-mNbits + 2);
    signed rmaxfexp = POW2(mEs) * (mNbits - 2);
    signed rfexp = MIN(MAX(xfexp + pfexp - shift + 1, rminfexp), rmaxfexp);

    bool rsign = isNeg() ^ p.isNeg();
    signed rreg = rfexp >> mEs; // floor(rfexp / 2^mEs)
    unsigned rexp = rfexp - POW2(mEs) * rreg;
    POSIT_UTYPE rfrac = mfrac << (shift + 1);

    return Posit(buildBits(rsign, rreg, rexp, rfrac), mNbits, mEs, false);
}

Posit Posit::div(Posit& p)
{
    // no loss on reciprocation!
    Posit rp = p.rec();

    return mul(rp);
}

bool Posit::eq(Posit& p)
{
    return mBits == p.mBits;
}

bool Posit::gt(Posit& p)
{
    if (isInf() || p.isInf()) {
        return false;
    }

    return mBits > p.mBits;
}

bool Posit::ge(Posit& p)
{
    return gt(p) || eq(p);
}

bool Posit::lt(Posit& p)
{
    return !gt(p) && !eq(p);
}

bool Posit::le(Posit& p)
{
    return !gt(p);
}

void Posit::set(float n)
{
    union {
        float f;
        uint32_t bits;
    };

    switch (fpclassify(n)) {
    case FP_INFINITE:
        mBits = POSIT_INF;
        mNan = false;
        break;
    case FP_NAN:
        mNan = true;
        break;
    case FP_ZERO:
        mBits = POSIT_ZERO;
        mNan = false;
        break;
    case FP_SUBNORMAL:
        // TODO: support subnormals
        mBits = POSIT_ZERO;
        mNan = false;
        break;
    case FP_NORMAL:
        f = n;
        mBits = ieeeToBits(bits, 8, 23);
        mNan = false;
        break;
    }
}

void Posit::set(double n)
{
    union {
        double f;
        uint64_t bits;
    };

    switch (fpclassify(n)) {
    case FP_INFINITE:
        mBits = POSIT_INF;
        mNan = false;
        break;
    case FP_NAN:
        mNan = true;
        break;
    case FP_ZERO:
        mBits = POSIT_ZERO;
        mNan = false;
        break;
    case FP_SUBNORMAL:
        // TODO: support subnormals
        mBits = POSIT_ZERO;
        mNan = false;
        break;
    case FP_NORMAL:
        f = n;
        mBits = ieeeToBits(bits, 11, 52);
        mNan = false;
        break;
    }
}

float Posit::getFloat()
{
    if (isZero()) {
        return 0.f;
    } else if (isInf()) {
        return 1.f / 0.f;
    }

    Posit p = (isNeg() ? neg() : *this);

    return (isNeg() ? -1 : 1) *
           powf(2, POW2(mEs) * p.regime() + (signed)p.exponent()) *
           (1 + (float)p.fraction() / POW2(p.fs()));
}

double Posit::getDouble()
{
    if (isZero()) {
        return 0.0;
    } else if (isInf()) {
        return 1.0 / 0.0;
    }

    Posit p = (isNeg() ? neg() : *this);

    return (isNeg() ? -1 : 1) *
           pow(2, POW2(mEs) * p.regime() + (signed)p.exponent()) *
           (1 + (double)p.fraction() / POW2(p.fs()));
}

void Posit::setBits(POSIT_UTYPE bits)
{
    mBits = bits << (POSIT_SIZE - mNbits);
}

POSIT_UTYPE Posit::getBits()
{
    return mBits >> (POSIT_SIZE - mNbits);
}

void Posit::print()
{
    Posit p = isNeg() || isInf() ? neg() : *this;

    printf("{%d, %d} ", mNbits, mEs);

    for (signed i = POSIT_SIZE - 1; i >= (signed)(POSIT_SIZE - mNbits); i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" (%d) -> ", regime());
    printf(isNeg() || isInf() ? "-" : "+");

    for (signed i = POSIT_SIZE - ss() - 1;
         i >= (signed)(POSIT_SIZE - mNbits); i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i != (signed)(POSIT_SIZE - mNbits) &&
            (((unsigned)i == (POSIT_SIZE - ss() - p.rs())) ||
             ((unsigned)i == (POSIT_SIZE - ss() - p.rs() - mEs)))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}
