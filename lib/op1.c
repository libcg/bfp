#include "op1.h"
#include "op2.h"
#include "util.h"

static struct unpacked_t half(struct unpacked_t a)
{
    struct unpacked_t r = a;

    r.exp--;

    return r;
}

struct unpacked_t op1_sqrt(struct unpacked_t a)
{
    struct unpacked_t r = a;

    // initial guess: half exponent is the sqrt if we ignore fraction bits
    r.exp /= 2;

    for (int i = 0; i < 100; i++) {
        struct unpacked_t rn;

        // Newton-Raphson: rn = r - (r^2 - a) / (2 * r) = (r + a / r) / 2
        rn = half(op2_add(r, op2_div(a, r)));

        if (rn.exp == r.exp && rn.frac == r.frac) {
            break;
        }

        r = rn;
    }

    return r;
}
