#include "posit.h"

POSIT_UTYPE Posit::buildMask(unsigned size)
{
    POSIT_UTYPE mask = ~(POSIT_UTYPE)0;

    mask <<= POSIT_SIZE - size;
    mask >>= POSIT_SIZE - size;

    return mask;
}

Posit::Posit(unsigned nbits, unsigned es, bool nan) :
    mBits(0x0),
    mNbits(nbits),
    mEs(es),
    mNan(nan)
{
}

Posit::Posit(unsigned nbits, unsigned es) :
    Posit(nbits, es, false)
{
}

bool Posit::isZero()
{
    return mBits == 0;
}

bool Posit::isOne()
{
    POSIT_UTYPE mask = buildMask(mNbits - 1);

    return ((mBits & mask) == (POSIT_UTYPE)(1 << (mNbits - 2)));
}

bool Posit::isInf()
{
    return mBits == (POSIT_UTYPE)(1 << (mNbits - 1));
}

bool Posit::isNeg()
{
    return (mBits & (1 << (mNbits - 1))) && !isInf();
}

bool Posit::isNan()
{
    return mNan;
}

unsigned Posit::nbits()
{
    return mNbits;
}

unsigned Posit::rs()
{
    signed lastBit = -1;
    unsigned rs = 0;

    // find a bit that changes, ignoring sign bit
    for (signed i = mNbits - 2; i >= 0; i--) {
        bool bit = (mBits >> i) & 1;
        rs++;

        if (bit != lastBit && lastBit >= 0) {
            break;
        }

        lastBit = bit;
    }

    return rs;
}

unsigned Posit::es()
{
    unsigned efs = mNbits - 1 - rs();
    unsigned es = mEs < efs ? mEs : efs;

    return (es >= 0 ? es : 0);
}

unsigned Posit::fs()
{
    return mNbits - 1 - rs() - es();
}

Posit Posit::zero()
{
    return Posit(mNbits, mEs);
}

Posit Posit::inf()
{
    return zero().rec();
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
    p.mBits = ((mBits ^ mask) + 1) & mask;

    return p;
}

Posit Posit::rec()
{
    Posit p = Posit(mNbits, mEs);
    POSIT_UTYPE mask = buildMask(mNbits);

    // reverse all bits but the first one and add one
    p.mBits = ((mBits ^ (mask >> 1)) + 1) & mask;

    return p;
}

Posit Posit::add(Posit& p)
{
    // fast exit
    if (isZero()) {
        return p;
    } else if (p.isZero()) {
        return *this;
    } else if (neg().eq(p)) {
        return zero();
    } else if (isInf() && p.isInf()) {
        return nan();
    } else if (isInf() || p.isInf()) {
        return inf();
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
    }

    // TODO implement
    return *this;
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

    // use sign bit of the base type to perform comparison
    POSIT_STYPE xbits = mBits << (POSIT_SIZE - mNbits);
    POSIT_STYPE pbits = p.mBits << (POSIT_SIZE - mNbits);

    return xbits > pbits;
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

void Posit::set(int n)
{
    // TODO implement
}

void Posit::set(float n)
{
    // TODO implement
}

void Posit::set(double n)
{
    // TODO implement
}

void Posit::set(std::string& n)
{
    // TODO implement
}

void Posit::get(int& n)
{
    // TODO implement
    n = 0;
}

void Posit::get(float& n)
{
    // TODO implement
    n = 0.f;
}

void Posit::get(double& n)
{
    // TODO implement
    n = 0.;
}

void Posit::get(std::string& n)
{
    // TODO implement
    n = "0";
}

void Posit::setBits(POSIT_UTYPE bits)
{
    mBits = bits;
}

POSIT_UTYPE Posit::getBits()
{
    return mBits;
}

void Posit::print()
{
    Posit p = isNeg() ? neg() : *this;

    printf("{%d, %d} ", mNbits, mEs);

    for (signed i = mNbits - 1; i >= 0; i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" -> ");
    printf(isNeg() || isInf() ? "-" : "+");

    for (signed i = mNbits - 2; i >= 0; i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i > 0 &&
            (((unsigned)i == (mNbits - 1 - p.rs())) ||
             ((unsigned)i == (mNbits - 1 - p.rs() - mEs)))) {
            printf(" ");
        }
    }

    printf("\n");
}
