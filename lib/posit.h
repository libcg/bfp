#ifndef __POSIT_H
#define __POSIT_H

#include <string>
#include <cstdint>

#define POSIT_UTYPE     uint32_t
#define POSIT_STYPE     int32_t
#define POSIT_SIZE      (8 * sizeof(POSIT_UTYPE))
#define POSIT_ZERO      (POSIT_UTYPE)0x00000000
#define POSIT_ONE       (POSIT_UTYPE)0x40000000
#define POSIT_MONE      (POSIT_UTYPE)0xC0000000
#define POSIT_INF       (POSIT_UTYPE)0x80000000
#define POSIT_MASK      (POSIT_UTYPE)0xFFFFFFFF

class Posit {
private:
    POSIT_UTYPE mBits;
    unsigned mNbits;
    unsigned mEs;
    bool mNan;

    POSIT_UTYPE buildMask(unsigned size);

    Posit(POSIT_UTYPE bits, unsigned nbits, unsigned es, bool nan);
    Posit(unsigned nbits, unsigned es, bool nan);

public:
    Posit(unsigned nbits, unsigned es);

    bool isZero();                  // check for 0
    bool isOne();                   // check for +/- 1
    bool isInf();                   // check for +/- infinity
    bool isNeg();                   // check for negative
    bool isNan();                   // check for NaN

    unsigned nbits();               // size in bits
    unsigned rs();                  // regime size in bits
    unsigned es();                  // exponent size in bits
    unsigned fs();                  // fraction size in bits
    unsigned useed();               // value at 45°

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

    void set(int n);                // x = n
    void set(float n);              // x = n
    void set(double n);             // x = n
    void set(std::string& n);       // x = n

    void get(int& n);               // n = x
    void get(float& n);             // n = x
    void get(double& n);            // n = x
    void get(std::string& n);       // n = x

    // debug
    void setBits(POSIT_UTYPE bits);
    POSIT_UTYPE getBits();
    void print();
};

#endif
