#ifndef __POSIT_H
#define __POSIT_H

#include "posit_types.h"

class Posit {
private:
    POSIT_UTYPE mBits;
    int mNbits;
    int mEs;
    bool mNan;

    static int sDefaultNbits;
    static int sDefaultEs;

    Posit(POSIT_UTYPE bits, int nbits, int es, bool nan);
    Posit(int nbits, int es, bool nan);

public:
    static void setDefault(int nbits, int es);

    Posit();
    Posit(double v);
    Posit(int nbits, int es);

    bool isZero() const;                  // check for 0
    bool isOne() const;                   // check for +/- 1
    bool isInf() const;                   // check for +/- infinity
    bool isNeg() const;                   // check for negative
    bool isNan() const;                   // check for NaN

    int nbits() const;                    // size in bits
    int ss() const;                       // sign size in bits
    int rs() const;                       // regime size in bits
    int es() const;                       // exponent size in bits
    int fs() const;                       // fraction size in bits

    Posit zero() const;                   // 0
    Posit one() const;                    // 1
    Posit inf() const;                    // +/-inf
    Posit nan() const;                    // NaN

    Posit neg() const;                    // -x
    Posit rec() const;                    // /x as 1 / x

    Posit add(const Posit& p) const;      // x + p
    Posit sub(const Posit& p) const;      // x - p
    Posit mul(const Posit& p) const;      // x * p
    Posit div(const Posit& p) const;      // x / p

    bool eq(const Posit& p) const;        // x == p
    bool gt(const Posit& p) const;        // x > p
    bool ge(const Posit& p) const;        // x >= p
    bool lt(const Posit& p) const;        // x < p
    bool le(const Posit& p) const;        // x <= p

    void set(float n);                    // x = n
    void set(double n);                   // x = n

    float getFloat() const;               // n = x
    double getDouble() const;             // n = x

    // debug
    void setBits(POSIT_UTYPE bits);
    POSIT_UTYPE getBits();
    void print();
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
