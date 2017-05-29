#ifndef __POSIT_PACK_H
#define __POSIT_PACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "posit_types.h"

struct unpkd_posit_t
{
    bool neg;
    int reg;
    POSIT_UTYPE exp;
    POSIT_UTYPE frac;
};

POSIT_UTYPE pack_posit(struct unpkd_posit_t p, int nbits, int es);

#ifdef __cplusplus
}
#endif

#endif
