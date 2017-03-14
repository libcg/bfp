#include <cassert>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "test.h"

static POSIT_STYPE div_table[8][8];
static POSIT_STYPE mul_table[8][8] = {
//        0    1/4      1      4    inf     -4     -1   -1/4
    { 0b000, 0b000, 0b000, 0b000,   NAN, 0b000, 0b000, 0b000 }, // 0
    { 0b000, 0b001, 0b001, 0b010, 0b100, 0b110, 0b111, 0b111 }, // 1/4
    { 0b000, 0b001, 0b010, 0b011, 0b100, 0b101, 0b110, 0b111 }, // 1
    { 0b000, 0b010, 0b011, 0b011, 0b100, 0b101, 0b101, 0b110 }, // 4
    {   NAN, 0b100, 0b100, 0b100, 0b100, 0b100, 0b100, 0b100 }, // inf
    { 0b000, 0b110, 0b101, 0b101, 0b100, 0b011, 0b011, 0b010 }, // -4
    { 0b000, 0b111, 0b110, 0b101, 0b100, 0b011, 0b010, 0b001 }, // -1
    { 0b000, 0b111, 0b111, 0b110, 0b100, 0b010, 0b001, 0b001 }, // -1/4
};

static void TestP3Mul(CuTest* tc)
{
    TEST_OP2(mul, 3, 1)
}

static void TestP3Div(CuTest* tc)
{
    TEST_OP2(div, 3, 1)
}

CuSuite* TestP3GetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    genDivTable(div_table[0], mul_table[0], 8);

    SUITE_ADD_TEST(suite, TestP3Mul);
    SUITE_ADD_TEST(suite, TestP3Div);

    return suite;
}
