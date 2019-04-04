#include <cassert>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "test.h"

#define NAR     0b10

static POSIT_STYPE sub_table[4][4];
static POSIT_STYPE add_table[4][4] = {
//       0     1   NaR    -1
    { 0b00, 0b01,  NAR, 0b11 }, // 0
    { 0b01, 0b01,  NAR, 0b00 }, // 1
    {  NAR,  NAR,  NAR,  NAR }, // NaR
    { 0b11, 0b00,  NAR, 0b11 }, // -1
};

static POSIT_STYPE div_table[4][4];
static POSIT_STYPE mul_table[4][4] = {
//       0     1   NaR    -1
    { 0b00, 0b00,  NAR, 0b00 }, // 0
    { 0b00, 0b01,  NAR, 0b11 }, // 1
    {  NAR,  NAR,  NAR,  NAR }, // NaR
    { 0b00, 0b11,  NAR, 0b01 }, // -1
};

static void TestP2Zero(CuTest* tc)
{
    Posit p = Posit(0, 2, 0);
    Posit zero = p.zero();
    Posit nar = p.nar();
    Posit neg = p.neg();
    Posit rec = p.rec();

    CuAssertTrue(tc, p.isZero());
    CuAssertTrue(tc, !p.isNar());
    CuAssertTrue(tc, !p.isNeg());
    CuAssertTrue(tc, p.nbits() == 2);
    CuAssertTrue(tc, p.rs() == 1);
    CuAssertTrue(tc, p.es() == 0);
    CuAssertTrue(tc, p.fs() == 0);
    CuAssertTrue(tc, p.eq(zero));
    CuAssertTrue(tc, !p.eq(nar));
    CuAssertTrue(tc, p.eq(neg));
    CuAssertTrue(tc, rec.isNar());
}

static void TestP2Add(CuTest* tc)
{
    TEST_OP2(add, 2, 0)
}

static void TestP2Sub(CuTest* tc)
{
    TEST_OP2(sub, 2, 0)
}

static void TestP2Mul(CuTest* tc)
{
    TEST_OP2(mul, 2, 0)
}

static void TestP2Div(CuTest* tc)
{
    TEST_OP2(div, 2, 0)
}

static void TestP2Is(CuTest* tc)
{
    Posit p = Posit(0, 2, 0);
    Posit zero = p.zero();
    Posit one = p.one();
    Posit nar = p.nar();
    Posit mone = p.one().neg();

    CuAssertTrue(tc, zero.isZero());
    CuAssertTrue(tc, !one.isZero());
    CuAssertTrue(tc, !nar.isZero());
    CuAssertTrue(tc, !mone.isZero());

    CuAssertTrue(tc, !zero.isNar());
    CuAssertTrue(tc, !one.isNar());
    CuAssertTrue(tc, nar.isNar());
    CuAssertTrue(tc, !mone.isNar());

    CuAssertTrue(tc, !zero.isNeg());
    CuAssertTrue(tc, !one.isNeg());
    CuAssertTrue(tc, !nar.isNeg());
    CuAssertTrue(tc, mone.isNeg());

    CuAssertTrue(tc, !zero.isNar());
    CuAssertTrue(tc, !one.isNar());
    CuAssertTrue(tc, nar.isNar());
    CuAssertTrue(tc, !mone.isNar());
}

CuSuite* TestP2GetSuite(void)
{
    CuSuite* suite = CuSuiteNew();

    genSubTable(sub_table[0], add_table[0], 4);
    genDivTable(div_table[0], mul_table[0], 4);

    SUITE_ADD_TEST(suite, TestP2Zero);
    SUITE_ADD_TEST(suite, TestP2Add);
    SUITE_ADD_TEST(suite, TestP2Sub);
    SUITE_ADD_TEST(suite, TestP2Mul);
    SUITE_ADD_TEST(suite, TestP2Div);
    SUITE_ADD_TEST(suite, TestP2Is);

    return suite;
}
