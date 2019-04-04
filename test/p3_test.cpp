#include <cassert>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "test.h"

#define NAR     0b100

static POSIT_STYPE sub_table[8][8];
static POSIT_STYPE add_table[8][8] = {
//        0    1/4      1      4    NaR     -4     -1   -1/4
    { 0b000, 0b001, 0b010, 0b011,   NAR, 0b101, 0b110, 0b111 }, // 0
    { 0b001, 0b001, 0b010, 0b011,   NAR, 0b101, 0b110, 0b000 }, // 1/4
    { 0b010, 0b010, 0b010, 0b011,   NAR, 0b101, 0b000, 0b010 }, // 1
    { 0b011, 0b011, 0b011, 0b011,   NAR, 0b000, 0b011, 0b011 }, // 4
    {   NAR,   NAR,   NAR,   NAR,   NAR,   NAR,   NAR,   NAR }, // NaR
    { 0b101, 0b101, 0b101, 0b000,   NAR, 0b101, 0b101, 0b101 }, // -4
    { 0b110, 0b110, 0b000, 0b011,   NAR, 0b101, 0b110, 0b110 }, // -1
    { 0b111, 0b000, 0b010, 0b011,   NAR, 0b101, 0b110, 0b111 }, // -1/4
};

static POSIT_STYPE div_table[8][8];
static POSIT_STYPE mul_table[8][8] = {
//        0    1/4      1      4    NaR     -4     -1   -1/4
    { 0b000, 0b000, 0b000, 0b000,   NAR, 0b000, 0b000, 0b000 }, // 0
    { 0b000, 0b001, 0b001, 0b010,   NAR, 0b110, 0b111, 0b111 }, // 1/4
    { 0b000, 0b001, 0b010, 0b011,   NAR, 0b101, 0b110, 0b111 }, // 1
    { 0b000, 0b010, 0b011, 0b011,   NAR, 0b101, 0b101, 0b110 }, // 4
    {   NAR,   NAR,   NAR,   NAR,   NAR,   NAR,   NAR,   NAR }, // NaR
    { 0b000, 0b110, 0b101, 0b101,   NAR, 0b011, 0b011, 0b010 }, // -4
    { 0b000, 0b111, 0b110, 0b101,   NAR, 0b011, 0b010, 0b001 }, // -1
    { 0b000, 0b111, 0b111, 0b110,   NAR, 0b010, 0b001, 0b001 }, // -1/4
};

static void TestP3Add(CuTest* tc)
{
    TEST_OP2(add, 3, 1)
}

static void TestP3Sub(CuTest* tc)
{
    TEST_OP2(sub, 3, 1)
}

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

    genSubTable(sub_table[0], add_table[0], 8);
    genDivTable(div_table[0], mul_table[0], 8);

    SUITE_ADD_TEST(suite, TestP3Add);
    SUITE_ADD_TEST(suite, TestP3Sub);
    SUITE_ADD_TEST(suite, TestP3Mul);
    SUITE_ADD_TEST(suite, TestP3Div);

    return suite;
}
