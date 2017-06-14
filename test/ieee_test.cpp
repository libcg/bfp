#include "test.h"

#define EPSILON     0.0001

static void TestIeeeFloat(CuTest* tc)
{
    Posit p = Posit(32, 3);

    for (float i = 0.f; i < 1000.f; i += 0.001f) {
        p.set(i);
        float r = p.getFloat();
        CuAssertTrue(tc, i == r);
    }
}

static void TestIeeeDouble(CuTest* tc)
{
    Posit p = Posit(32, 3);

    for (double i = 0.0; i < 1000.0; i += 0.001) {
        p.set(i);
        double r = p.getDouble();
        CuAssertTrue(tc, i - EPSILON < r && r < i + EPSILON);
    }
}

CuSuite* TestIeeeGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestIeeeFloat);
    SUITE_ADD_TEST(suite, TestIeeeDouble);

    return suite;
}
