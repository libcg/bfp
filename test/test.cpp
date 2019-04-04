#include <cstdio>

#include "test.h"

CuSuite* TestP2GetSuite();
CuSuite* TestP3GetSuite();
CuSuite* TestIeeeGetSuite();

void genSubTable(POSIT_STYPE *sub_table, POSIT_STYPE *add_table, int size)
{
    for (int i = 0; i < size; i++) {
        int k;

        if (i == 0) {
            k = 0;
        } else {
            k = size / 2 - (i - size / 2);
        }

        for (int j = 0; j < size; j++) {
            sub_table[k * size + j] = add_table[i * size + j];
        }
    }
}

void genDivTable(POSIT_STYPE *div_table, POSIT_STYPE *mul_table, int size)
{
    for (int i = 0; i < size; i++) {
        int k;

        if (i <= size / 2) {
            k = size / 2 - i;
        } else {
            k = size - (i - size / 2);
        }

        for (int j = 0; j < size; j++) {
            if (k == size / 2) {
                // TODO explain
                div_table[k * size + j] = mul_table[size / 2 * size + j];
            } else {
                div_table[k * size + j] = mul_table[i * size + j];
            }
        }
    }
}

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    CuSuiteAddSuite(suite, TestP2GetSuite());
    CuSuiteAddSuite(suite, TestP3GetSuite());
    CuSuiteAddSuite(suite, TestIeeeGetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s", output->buffer);
}

int main(int argc, char *argv[])
{
    RunAllTests();
}
