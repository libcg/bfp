#include "op1.h"
#include "op2.h"
#include "util.h"

static struct unpacked_t half(struct unpacked_t a)
{
    struct unpacked_t r = a;

    a.exp--;

    return a;
}

struct unpacked_t op1_sqrt(struct unpacked_t a)
{
    struct unpacked_t r = a;
    bool found = false;

    // initial guess: half exponent
    r.exp /= 2;

    for (int i = 0; !found && i < 100; i++) {
        struct unpacked_t rn;

        // Newton-Raphson: rn = r - (r^2 - a) / (2 * r)
        rn = op2_sub(r, half(op2_div(op2_sub(op2_mul(r, r), a), r)));
        found = (rn.exp == r.exp && rn.frac == r.frac);
        r = rn;
    }

    return r;
}
