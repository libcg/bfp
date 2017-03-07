#include "posit.h"

POSIT_UTYPE Posit::buildMask(unsigned size)
{
    return POSIT_MASK << (POSIT_SIZE - size);
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
    return mBits == POSIT_ONE || (mBits == (POSIT_ONE | POSIT_NEG));
}

bool Posit::isInf()
{
    return mBits == POSIT_INF;
}

bool Posit::isNeg()
{
    return mBits != POSIT_INF && (mBits & POSIT_NEG);
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
    for (signed i = POSIT_SIZE - 2; i >= POSIT_SIZE - mNbits; i--) {
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

unsigned Posit::useed()
{
    return 1 << (1 << mEs);
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
    p.mBits = ((mBits ^ (POSIT_MASK >> 1)) + 1) & mask;

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

    for (signed i = POSIT_SIZE - 1; i >= POSIT_SIZE - mNbits; i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" -> ");
    printf(isNeg() || isInf() ? "-" : "+");

    for (signed i = POSIT_SIZE - 2; i >= POSIT_SIZE - mNbits; i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i != POSIT_SIZE - mNbits &&
            (((unsigned)i == (POSIT_SIZE - 1 - p.rs())) ||
             ((unsigned)i == (POSIT_SIZE - 1 - p.rs() - mEs)))) {
            printf(" ");
        }
    }

    printf("\n");
}
