#include <cstdio>

#include "CuTest.h"

CuSuite* TestP2GetSuite();

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    CuSuiteAddSuite(suite, TestP2GetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s", output->buffer);
}

int main(int argc, char *argv[])
{
    RunAllTests();
}
