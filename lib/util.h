#ifndef __POSIT_UTIL_H
#define __POSIT_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "posit_types.h"

#ifdef __GNUC__
#if POSIT_WIDTH > 32
#error "CLZ doesn't support 64-bit"
#endif
#define CLZ(n) \
    ((n) == 0 ? 8 * sizeof(n) : __builtin_clz(n))
#endif

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
#define LSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) << (shift))

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
#define RSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) >> (shift))

#define POW2(n) \
    (LSHIFT(1, (n)))

#define FLOORDIV(a, b) \
    ((a) / (b) - ((a) % (b) < 0))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define LMASK(bits, size) \
    ((bits) & LSHIFT(POSIT_MASK, POSIT_WIDTH - (size)))

#define HIDDEN_BIT(frac) \
    (POSIT_MSB | RSHIFT((frac), 1))

bool util_is_zero(POSIT_UTYPE p);
bool util_is_nar(POSIT_UTYPE p);
bool util_is_neg(POSIT_UTYPE p);

int util_ss();
int util_rs(POSIT_UTYPE p, int nbits);
int util_es(POSIT_UTYPE p, int nbits, int es);
int util_fs(POSIT_UTYPE p, int nbits, int es);

POSIT_UTYPE util_neg(POSIT_UTYPE p, int nbits);

#ifdef __cplusplus
}
#endif

#endif
