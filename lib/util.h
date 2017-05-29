#ifndef __POSIT_UTIL_H
#define __POSIT_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "posit_types.h"

#ifdef __GNUC__
#define CLZ(n) \
    __builtin_clz(n)
#endif

#define POW2(n) \
    (1 << (n))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define LMASK(bits, size) \
    ((bits) & (POSIT_MASK << (POSIT_SIZE - (size))))

bool util_is_zero(POSIT_UTYPE p);
bool util_is_one(POSIT_UTYPE p);
bool util_is_inf(POSIT_UTYPE p);
bool util_is_neg(POSIT_UTYPE p);

int util_ss();
int util_rs(POSIT_UTYPE p, int nbits);
int util_es(POSIT_UTYPE p, int nbits, int es);
int util_fs(POSIT_UTYPE p, int nbits, int es);
int util_regime(POSIT_UTYPE p, int nbits);

POSIT_UTYPE util_neg(POSIT_UTYPE p, int nbits);
POSIT_UTYPE util_rec(POSIT_UTYPE p, int nbits, int es);

#ifdef __cplusplus
}
#endif

#endif
