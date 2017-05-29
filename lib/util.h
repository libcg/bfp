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

#ifdef __cplusplus
}
#endif

#endif
