#ifndef __POSIT_H
#define __POSIT_H

#include <string>
#include <cstdint>

#define POSIT_UTYPE     uint32_t
#define POSIT_STYPE     int32_t
#define POSIT_SIZE      (8 * sizeof(POSIT_UTYPE))

class Posit {
private:
    POSIT_UTYPE mBits;
    unsigned mNbits;
    unsigned mEs;

    POSIT_UTYPE buildMask(unsigned size);

public:
    Posit(unsigned nbits, unsigned es);

    bool isZero();                  // check for 0
    bool isInf();                   // check for +/- infinity
    bool isNeg();                   // check for negative

    unsigned nbits();               // size in bits
    unsigned rs();                  // regime size in bits
    unsigned es();                  // exponent size in bits
    unsigned fs();                  // fraction size in bits

    Posit zero();                   // 0
    Posit inf();                    // +/-inf

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
