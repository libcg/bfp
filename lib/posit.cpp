#include "posit.h"
#include "util.h"
#include "pack.h"
#include "op1.h"
#include "op2.h"

#include <cstdio>
#include <cmath>

using namespace std;

Posit::Posit(POSIT_UTYPE bits, int nbits, int es) :
    mBits(bits),
    mNbits(nbits),
    mEs(es)
{
}

Posit::Posit(int nbits, int es) :
    Posit(POSIT_ZERO, nbits, es)
{
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
        return nar();
    }

    return Posit(util_neg(mBits, mNbits), mNbits, mEs);
}

Posit Posit::rec() const
{
    if (isNar() || isZero()) {
        return nar();
    }

    return one().div(*this);
}

Posit Posit::sqrt() const
{
    if (isNar() || isNeg()) {
        return nar();
    } else if (isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(mBits, mNbits, mEs);
    unpacked_t up = op1_sqrt(aup);

    return Posit(pack_posit(up, mNbits, mEs), mNbits, mEs);
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

void Posit::set(Posit p)
{
    mBits = pack_posit(unpack_posit(p.mBits, p.mNbits, p.mEs), mNbits, mEs);
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
    mBits = LSHIFT(bits, POSIT_WIDTH - mNbits);
}

POSIT_UTYPE Posit::getBits()
{
    return RSHIFT(mBits, POSIT_WIDTH - mNbits);
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
        printf("%d", RSHIFT(mBits, i) & 1);
    }

    printf(" -> ");
    printf(isNeg() ? "-" : "+");

    for (int i = POSIT_WIDTH - ss() - 1; i >= POSIT_WIDTH - mNbits; i--) {
        printf("%d", RSHIFT(p.mBits, i) & 1);

        if (i != POSIT_WIDTH - mNbits &&
            ((i == POSIT_WIDTH - ss() - p.rs()) ||
             (i == POSIT_WIDTH - ss() - p.rs() - mEs))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}

Posit8::Posit8() :
    Posit(8, 0)
{

}

Posit8::Posit8(Posit v) :
    Posit8()
{
    set(v);
}

Posit8::Posit8(float v) :
    Posit8()
{
    set(v);
}

Posit8::Posit8(double v) :
    Posit8()
{
    set(v);
}

Posit16::Posit16() :
    Posit(16, 1)
{

}

Posit16::Posit16(Posit v) :
    Posit16()
{
    set(v);
}

Posit16::Posit16(float v) :
    Posit16()
{
    set(v);
}

Posit16::Posit16(double v) :
    Posit16()
{
    set(v);
}

Posit32::Posit32() :
    Posit(32, 2)
{

}

Posit32::Posit32(Posit v) :
    Posit32()
{
    set(v);
}

Posit32::Posit32(float v) :
    Posit32()
{
    set(v);
}

Posit32::Posit32(double v) :
    Posit32()
{
    set(v);
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
