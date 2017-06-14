#ifndef __POSIT_PACK_H
#define __POSIT_PACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "posit_types.h"

struct unpacked_t
{
    bool neg;
    POSIT_STYPE exp;
    POSIT_UTYPE frac;
};

POSIT_UTYPE pack_posit(struct unpacked_t up, int nbits, int es);
float pack_float(struct unpacked_t up);
double pack_double(struct unpacked_t up);

struct unpacked_t unpack_posit(POSIT_UTYPE p, int nbits, int es);
struct unpacked_t unpack_float(float f);
struct unpacked_t unpack_double(double f);

#ifdef __cplusplus
}
#endif

#endif
