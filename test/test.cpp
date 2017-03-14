#include <cstdio>

#include "test.h"

CuSuite* TestP2GetSuite();
CuSuite* TestP3GetSuite();

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
            div_table[k * size + j] = mul_table[i * size + j];
        }
    }
}

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    CuSuiteAddSuite(suite, TestP2GetSuite());
    CuSuiteAddSuite(suite, TestP3GetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s", output->buffer);
}

int main(int argc, char *argv[])
{
    RunAllTests();
}
