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

Posit::Posit(POSIT_UTYPE bits, int nbits, int es) :
    mBits(bits),
    mNbits(nbits),
    mEs(es)
{
}

Posit::Posit(int nbits, int es) :
    Posit(POSIT_NAR, nbits, es)
{
}

Posit::Posit() :
    Posit(sDefaultNbits, sDefaultEs)
{
}

Posit::Posit(double v) :
    Posit(sDefaultNbits, sDefaultEs)
{
    set(v);
}

bool Posit::isZero() const
{
    return util_is_zero(mBits);
}

bool Posit::isNar() const
{
    return util_is_nar(mBits);
}

bool Posit::isNeg() const
{
    return util_is_neg(mBits);
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
    return Posit(POSIT_ZERO, mNbits, mEs);
}

Posit Posit::one() const
{
    return Posit(POSIT_ONE, mNbits, mEs);
}

Posit Posit::nar() const
{
    return Posit(POSIT_NAR, mNbits, mEs);
}

Posit Posit::neg() const
{
    if (isNar()) {
        return *this;
    }

    return Posit(util_neg(mBits, mNbits), mNbits, mEs);
}

Posit Posit::rec() const
{
    if (isNar()) {
        return *this;
    }

    return one().div(*this);
}

Posit Posit::add(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero()) {
        return p;
    } else if (p.isZero()) {
        return *this;
    } else if (neg().eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_add(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs);
}

Posit Posit::sub(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero()) {
        return p.neg();
    } else if (p.isZero()) {
        return *this;
    } else if (eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_sub(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs);
}

Posit Posit::mul(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero() || p.isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_mul(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs);
}

Posit Posit::div(const Posit& p) const
{
    // fast exit
    if (isNar() || p.isNar() || p.isZero()) {
        return nar();
    } else if (isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t bup = unpack_posit(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_div(aup, bup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs);
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
    case FP_NAN:
        mBits = POSIT_NAR;
        break;
    case FP_ZERO:
        mBits = POSIT_ZERO;
        break;
    default:
        mBits = pack_posit(unpack_float(n), mNbits, mEs);
        break;
    }
}

void Posit::set(double n)
{
    switch (fpclassify(n)) {
    case FP_INFINITE:
    case FP_NAN:
        mBits = POSIT_NAR;
        break;
    case FP_ZERO:
        mBits = POSIT_ZERO;
        break;
    default:
        mBits = pack_posit(unpack_double(n), mNbits, mEs);
        break;
    }
}

float Posit::getFloat() const
{
    if (isZero()) {
        return 0.f;
    } else if (isNar()) {
        return 0.f / 0.f;
    }

    return pack_float(unpack_posit(mBits, mNbits, mEs));
}

double Posit::getDouble() const
{
    if (isZero()) {
        return 0.0;
    } else if (isNar()) {
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
    Posit p = (isNeg() ? neg() : *this);

    printf("{%d, %d} ", mNbits, mEs);

    if (isNar()) {
        printf("NaR\n");
        return;
    }

    for (int i = POSIT_WIDTH - 1; i >= POSIT_WIDTH - mNbits; i--) {
        printf("%d", (mBits >> i) & 1);
    }

    printf(" -> ");
    printf(isNeg() ? "-" : "+");

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
