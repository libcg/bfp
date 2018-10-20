#include "posit.h"
#include "util.h"
#include "pack.h"
#include "op2.h"

#include <cstdio>
#include <cmath>

using namespace std;

int Posit::sDefaultNbits = 32;
int Posit::sDefaultEs = 2;

void Posit::setDefault(int nbits, int es)
{
    sDefaultNbits = nbits;
    sDefaultEs = es;
}

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

Posit::Posit() : Posit(sDefaultNbits, sDefaultEs)
{
}

Posit::Posit(double v) : Posit(sDefaultNbits, sDefaultEs)
{
    set(v);
}

Posit::Posit(int nbits, int es) :
    Posit(nbits, es, false)
{
}

bool Posit::isZero() const
{
    return util_is_zero(mBits);
}

bool Posit::isOne() const
{
    return util_is_one(mBits);
}

bool Posit::isInf() const
{
    return util_is_inf(mBits);
}

bool Posit::isNeg() const
{
    return util_is_neg(mBits);
}

bool Posit::isNan() const
{
    return mNan;
}

int Posit::nbits() const
{
    return mNbits;
}

int Posit::ss() const
{
    return util_ss();
}

int Posit::rs() const
{
    return util_rs(mBits, mNbits);
}

int Posit::es() const
{
    return util_es(mBits, mNbits, mEs);
}

int Posit::fs() const
{
    return util_fs(mBits, mNbits, mEs);
}

Posit Posit::zero() const
{
    return Posit(POSIT_ZERO, mNbits, mEs, false);
}

Posit Posit::one() const
{
    return Posit(POSIT_ONE, mNbits, mEs, false);
}

Posit Posit::inf() const
{
    return Posit(POSIT_INF, mNbits, mEs, false);
}

Posit Posit::nan() const
{
    return Posit(mNbits, mEs, true);
}

Posit Posit::neg() const
{
    return Posit(util_neg(mBits, mNbits), mNbits, mEs, false);
}

Posit Posit::rec() const
{
    return Posit(POSIT_ONE, mNbits, mEs, false).div(*this);
}

Posit Posit::add(const Posit& p) const
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

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_add(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs, false);
}

Posit Posit::sub(const Posit& p) const
{
    // fast exit
    if (isZero()) {
        return p.neg();
    } else if (p.isZero()) {
        return *this;
    } else if (isInf() && p.isInf()) {
        return nan();
    } else if (isInf() || p.isInf()) {
        return inf();
    } else if (eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_sub(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs, false);
}

Posit Posit::mul(const Posit& p) const
{
    // fast exit
    if (isZero()) {
        return (p.isInf() ? nan() : zero());
    } else if (p.isZero()) {
        return (isInf() ? nan() : zero());
    } else if (isInf() || p.isInf()) {
        return inf();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_mul(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs, false);
}

Posit Posit::div(const Posit& p) const
{
    // fast exit
    if (isInf()) {
        return (p.isInf() ? nan() : inf());
    } else if (p.isZero()) {
        return (isZero() ? nan() : inf());
    } else if (isZero() || p.isInf()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_div(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs, false);
}

bool Posit::eq(const Posit& p) const
{
    return mBits == p.mBits;
}

bool Posit::gt(const Posit& p) const
{
    return (POSIT_STYPE)mBits > (POSIT_STYPE)p.mBits;
}

bool Posit::ge(const Posit& p) const
{
    return gt(p) || eq(p);
}

bool Posit::lt(const Posit& p) const
{
    return !gt(p) && !eq(p);
}

bool Posit::le(const Posit& p) const
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
    default:
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
    default:
        mBits = pack_posit(unpack_double(n), mNbits, mEs);
        mNan = false;
        break;
    }
}

float Posit::getFloat() const
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

double Posit::getDouble() const
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
    mBits = bits << (POSIT_WIDTH - mNbits);
}

POSIT_UTYPE Posit::getBits()
{
    return mBits >> (POSIT_WIDTH - mNbits);
}

void Posit::print()
{
    Posit p = isNeg() || isInf() ? neg() : *this;

    printf("{%d, %d} ", mNbits, mEs);

    for (int i = POSIT_WIDTH - 1; i >= POSIT_WIDTH - mNbits; i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" -> ");
    printf(isNeg() || isInf() ? "-" : "+");

    for (int i = POSIT_WIDTH - ss() - 1; i >= POSIT_WIDTH - mNbits; i--) {
        printf("%d", (p.mBits >> i) & 1);

        if (i != POSIT_WIDTH - mNbits &&
            ((i == POSIT_WIDTH - ss() - p.rs()) ||
             (i == POSIT_WIDTH - ss() - p.rs() - mEs))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}

Posit operator+(const Posit& a, const Posit& b)
{
    return a.add(b);
}

Posit operator-(const Posit& a, const Posit& b)
{
    return a.sub(b);
}

Posit operator*(const Posit& a, const Posit& b)
{
    return a.mul(b);
}

Posit operator/(const Posit& a, const Posit& b)
{
    return a.div(b);
}

Posit operator-(const Posit& a)
{
    return a.neg();
}

bool operator<(const Posit&a , const Posit& b)
{
    return a.lt(b);
}

bool operator<=(const Posit&a , const Posit& b)
{
    return a.le(b);
}

bool operator>(const Posit&a , const Posit& b)
{
    return a.gt(b);
}

bool operator>=(const Posit&a , const Posit& b)
{
    return a.ge(b);
}
bool operator==(const Posit&a , const Posit& b)
{
    return a.eq(b);
}

bool operator!=(const Posit&a , const Posit& b)
{
    return !a.eq(b);
}
