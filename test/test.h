#ifndef __TEST_H
#define __TEST_H

#include "CuTest.h"
#include "posit.h"
#include <stdio.h>

#define TEST_OP2(OP, NBITS, ES)                                                \
    Posit a = Posit(NBITS, ES);                                                \
    Posit b = Posit(NBITS, ES);                                                \
                                                                               \
    for (int i = 0; i < (1 << NBITS); i++) {                                   \
        a.setBits(i);                                                          \
        for (int j = 0; j < (1 << NBITS); j++) {                               \
            b.setBits(j);                                                      \
                                                                               \
            Posit c = a.OP(b);                                                 \
                                                                               \
            CuAssertTrue(tc, c.getBits() == (POSIT_UTYPE)OP ## _table[j][i]);  \
        }                                                                      \
    }

void genSubTable(POSIT_STYPE *sub_table, POSIT_STYPE *add_table, int size);
void genDivTable(POSIT_STYPE *div_table, POSIT_STYPE *mul_table, int size);

#endif
