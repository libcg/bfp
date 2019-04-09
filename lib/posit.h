#ifndef __POSIT_H
#define __POSIT_H

#include "posit_types.h"

class Posit {
private:
    POSIT_UTYPE mBits;
    int mNbits;
    int mEs;

public:
    Posit(POSIT_UTYPE bits, int nbits, int es);
    Posit(int nbits, int es);

    bool isZero() const;                  // check for 0
    bool isNar() const;                   // check for NaR
    bool isNeg() const;                   // check for negative

    int nbits() const;                    // size in bits
    int ss() const;                       // sign size in bits
    int rs() const;                       // regime size in bits
    int es() const;                       // exponent size in bits
    int fs() const;                       // fraction size in bits

    Posit zero() const;                   // 0
    Posit one() const;                    // 1
    Posit nar() const;                    // NaR

    Posit neg() const;                    // -x
    Posit rec() const;                    // 1 / x
    Posit sqrt() const;                   // sqrt(x)

    Posit add(const Posit& p) const;      // x + p
    Posit sub(const Posit& p) const;      // x - p
    Posit mul(const Posit& p) const;      // x * p
    Posit div(const Posit& p) const;      // x / p

    bool eq(const Posit& p) const;        // x == p
    bool gt(const Posit& p) const;        // x > p
    bool ge(const Posit& p) const;        // x >= p
    bool lt(const Posit& p) const;        // x < p
    bool le(const Posit& p) const;        // x <= p

    void set(Posit p);                    // x = p
    void set(float n);                    // x = n
    void set(double n);                   // x = n

    float getFloat() const;               // n = x
    double getDouble() const;             // n = x

    // debug
    void setBits(POSIT_UTYPE bits);
    POSIT_UTYPE getBits();
    void print();
};

class Posit8 : public Posit
{
public:
    Posit8();
    Posit8(Posit v);
    Posit8(float v);
    Posit8(double v);
};

class Posit16 : public Posit
{
public:
    Posit16();
    Posit16(Posit v);
    Posit16(float v);
    Posit16(double v);
};

class Posit32 : public Posit
{
public:
    Posit32();
    Posit32(Posit v);
    Posit32(float v);
    Posit32(double v);
};

Posit operator+(const Posit& a, const Posit& b);
Posit operator-(const Posit& a, const Posit& b);
Posit operator*(const Posit& a, const Posit& b);
Posit operator/(const Posit& a, const Posit& b);

Posit operator-(const Posit& a);

bool operator<(const Posit&a , const Posit& b);
bool operator<=(const Posit&a , const Posit& b);
bool operator>(const Posit&a , const Posit& b);
bool operator>=(const Posit&a , const Posit& b);
bool operator==(const Posit&a , const Posit& b);
bool operator!=(const Posit&a , const Posit& b);

#endif
