#include "posit.h"
#include "util.h"
#include "pack.h"
#include "op2.h"

#include <cstdio>
#include <cmath>

using namespace std;

Posit::Posit(POSIT_UTYPE bits, int nbits, int es, bool nan) :
    mBits(bits),
    mNbits(nbits),
    mEs(es),
    mNan(nan)
{
}

Posit::Posit(int nbits, int es, bool nan) :
    Posit(0, nbits, es, nan)
{
}

Posit::Posit(int nbits, int es) :
    Posit(nbits, es, false)
{
}

bool Posit::isZero()
{
    return util_is_zero(mBits);
}

bool Posit::isOne()
{
    return util_is_one(mBits);
}

bool Posit::isInf()
{
    return util_is_inf(mBits);
}

bool Posit::isNeg()
{
    return util_is_neg(mBits);
}

bool Posit::isNan()
{
    return mNan;
}

int Posit::nbits()
{
    return mNbits;
}

int Posit::ss()
{
    return util_ss();
}

int Posit::rs()
{
    return util_rs(mBits, mNbits);
}

int Posit::es()
{
    return util_es(mBits, mNbits, mEs);
}

int Posit::fs()
{
    return util_fs(mBits, mNbits, mEs);
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
    return Posit(util_neg(mBits, mNbits), mNbits, mEs, false);
}

Posit Posit::rec()
{
    return Posit(util_rec(mBits, mNbits, mEs), mNbits, mEs, false);
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

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_mul(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs, false);
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
        mBits = pack_posit(unpack_float(n), mNbits, mEs);
        mNan = false;
        break;
    }
}

void Posit::set(double n)
{
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
        mBits = pack_posit(unpack_double(n), mNbits, mEs);
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
    } else if (isNan()) {
        return 0.f / 0.f;
    }

    return pack_float(unpack_posit(mBits, mNbits, mEs));
}

double Posit::getDouble()
{
    if (isZero()) {
        return 0.0;
    } else if (isInf()) {
        return 1.0 / 0.0;
    } else if (isNan()) {
        return 0.0 / 0.0;
    }

    return pack_double(unpack_posit(mBits, mNbits, mEs));
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

    for (int i = POSIT_SIZE - 1; i >= POSIT_SIZE - mNbits; i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" -> ");
    printf(isNeg() || isInf() ? "-" : "+");

    for (int i = POSIT_SIZE - ss() - 1; i >= POSIT_SIZE - mNbits; i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i != POSIT_SIZE - mNbits &&
            ((i == POSIT_SIZE - ss() - p.rs()) ||
             (i == POSIT_SIZE - ss() - p.rs() - mEs))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}
