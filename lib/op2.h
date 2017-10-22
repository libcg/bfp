#ifndef __POSIT_OP2_H
#define __POSIT_OP2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pack.h"

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_add(struct unpacked_t a, struct unpacked_t b);
struct unpacked_t op2_sub(struct unpacked_t a, struct unpacked_t b);

#ifdef __cplusplus
}
#endif

#endif
