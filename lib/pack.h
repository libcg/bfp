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

POSIT_UTYPE pack_posit(struct unpkd_posit_t up, int nbits, int es);
float pack_float(struct unpkd_posit_t up, int es);
double pack_double(struct unpkd_posit_t up, int es);

struct unpkd_posit_t unpack_posit(POSIT_UTYPE p, int nbits, int es);

#ifdef __cplusplus
}
#endif

#endif
