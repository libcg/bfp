#include "posit.h"

#include <cstdio>

int main(int argc, char *argv[])
{
    auto p = Posit(5, 1);

    for (unsigned i = 0; i < (unsigned)(1 << p.nbits()); i++) {
        p.setBits(i);
        p.print();
    }

    return 0;
}
