#include "posit.h"

#include <cmath>
#include <sstream>

#define POW2(n) \
    (1 << n)

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
    return POW2(POW2(mEs));
}

signed Posit::regime()
{
    POSIT_UTYPE bits = (isNeg() ? neg().mBits : mBits) << 1;
    POSIT_UTYPE himask = 1 << (POSIT_SIZE - 1);
    signed r = 0;

    // out of bounds regime for error handling
    if (isZero()) {
        return -mNbits + 1;
    } else if (isInf()) {
        return mNbits - 1;
    }

    if (bits & himask) {    // >0
        while (1) {
            bits <<= 1;
            if (!(bits & himask))
                break;
            r++;
        }
    } else {                // <=0
        while (1) {
            bits <<= 1;
            r--;
            if (bits & himask)
                break;
        }
    }

    return r;
}

POSIT_UTYPE Posit::exponent()
{
    POSIT_UTYPE expBits = (mBits & (buildMask(mEs) >> (1 + rs())));

    return expBits >> (POSIT_SIZE - mNbits + fs());
}

POSIT_UTYPE Posit::fraction()
{
    POSIT_UTYPE fracBits = (mBits & (buildMask(fs()) >> (1 + rs() + es())));

    return fracBits >> (POSIT_SIZE - mNbits);
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

int Posit::getInt()
{
    return (int)getFloat();
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
           powf(p.useed(), p.regime()) *
           POW2(p.exponent()) *
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
           pow(p.useed(), p.regime()) *
           POW2(p.exponent()) *
           (1 + (double)p.fraction() / POW2(p.fs()));
}

std::string Posit::getString()
{
    std::ostringstream strs;

    strs << getDouble();

    return strs.str();
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

    printf(" (%d) -> ", regime());
    printf(isNeg() || isInf() ? "-" : "+");

    for (signed i = POSIT_SIZE - 2; i >= POSIT_SIZE - mNbits; i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i != POSIT_SIZE - mNbits &&
            (((unsigned)i == (POSIT_SIZE - 1 - p.rs())) ||
             ((unsigned)i == (POSIT_SIZE - 1 - p.rs() - mEs)))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}
