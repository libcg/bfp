#ifndef __POSIT_H
#define __POSIT_H

#include "posit_types.h"

class Posit {
private:
    POSIT_UTYPE mBits;
    int mNbits;
    int mEs;
    bool mNan;

    Posit(POSIT_UTYPE bits, int nbits, int es, bool nan);
    Posit(int nbits, int es, bool nan);

public:
    Posit(int nbits, int es);

    bool isZero();                  // check for 0
    bool isOne();                   // check for +/- 1
    bool isInf();                   // check for +/- infinity
    bool isNeg();                   // check for negative
    bool isNan();                   // check for NaN

    int nbits();                    // size in bits
    int ss();                       // sign size in bits
    int rs();                       // regime size in bits
    int es();                       // exponent size in bits
    int fs();                       // fraction size in bits
    int useed();                    // value at 45°

    Posit zero();                   // 0
    Posit one();                    // 1
    Posit inf();                    // +/-inf
    Posit nan();                    // NaN

    Posit neg();                    // -x
    Posit rec();                    // /x

    Posit add(Posit& p);            // x + p
    Posit sub(Posit& p);            // x - p as x + -p
    Posit mul(Posit& p);            // x * p
    Posit div(Posit& p);            // x / p as x * /p

    bool eq(Posit& p);              // x == p
    bool gt(Posit& p);              // x > p
    bool ge(Posit& p);              // x >= p
    bool lt(Posit& p);              // x < p
    bool le(Posit& p);              // x <= p

    void set(float n);              // x = n
    void set(double n);             // x = n

    float getFloat();               // n = x
    double getDouble();             // n = x

    // debug
    void setBits(POSIT_UTYPE bits);
    POSIT_UTYPE getBits();
    void print();
};

#endif
