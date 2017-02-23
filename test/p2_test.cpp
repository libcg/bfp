#include <cassert>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "CuTest.h"
#include "posit.h"

#define TEST_OP2(op)                                                           \
    Posit a = Posit(2, 0);                                                     \
    Posit b = Posit(2, 0);                                                     \
                                                                               \
    for (int i = 0; i < 4; i++) {                                              \
        a.setBits(i);                                                          \
        for (int j = 0; j < 4; j++) {                                          \
            b.setBits(j);                                                      \
                                                                               \
            Posit c = a.op(b);                                                 \
                                                                               \
            if (op ## _table[j][i] == -1) {                                    \
                CuAssertTrue(tc, c.isNan());                                   \
            } else {                                                           \
                CuAssertTrue(tc, !c.isNan());                                  \
                CuAssertTrue(tc, c.getBits() ==                                \
                                 (POSIT_UTYPE)op ## _table[j][i]);             \
            }                                                                  \
        }                                                                      \
    }

static POSIT_STYPE add_table[4][4] = {
//       0     1   inf    -1
    { 0b00, 0b01, 0b10, 0b11 }, // 0
    { 0b01, 0b01, 0b10, 0b00 }, // 1
    { 0b10, 0b10,   -1, 0b10 }, // inf
    { 0b11, 0b00, 0b10, 0b11 }, // -1
};

static POSIT_STYPE sub_table[4][4] = {
//       0     1   inf    -1
    { 0b00, 0b01, 0b10, 0b11 }, // 0
    { 0b11, 0b00, 0b10, 0b11 }, // 1
    { 0b10, 0b10,   -1, 0b10 }, // inf
    { 0b01, 0b01, 0b10, 0b00 }, // -1
};

static POSIT_STYPE mul_table[4][4] = {
//       0     1   inf    -1
    { 0b00, 0b00,   -1, 0b00 }, // 0
    { 0b00, 0b01, 0b10, 0b11 }, // 1
    {   -1, 0b10, 0b10, 0b10 }, // inf
    { 0b00, 0b11, 0b10, 0b01 }, // -1
};

static POSIT_STYPE div_table[4][4] = {
//       0     1   inf    -1
    {   -1, 0b10, 0b10, 0b10 }, // 0
    { 0b00, 0b01, 0b10, 0b11 }, // 1
    { 0b00, 0b00,   -1, 0b00 }, // inf
    { 0b00, 0b11, 0b10, 0b01 }, // -1
};

static void TestP2Zero(CuTest* tc)
{
    Posit p = Posit(2, 0);
    Posit zero = p.zero();
    Posit inf = p.inf();
    Posit neg = p.neg();
    Posit rec = p.rec();

    CuAssertTrue(tc, p.isZero());
    CuAssertTrue(tc, !p.isInf());
    CuAssertTrue(tc, !p.isNeg());
    CuAssertTrue(tc, p.nbits() == 2);
    CuAssertTrue(tc, p.rs() == 1);
    CuAssertTrue(tc, p.es() == 0);
    CuAssertTrue(tc, p.fs() == 0);
    CuAssertTrue(tc, p.eq(zero));
    CuAssertTrue(tc, !p.eq(inf));
    CuAssertTrue(tc, p.eq(neg));
    CuAssertTrue(tc, rec.isInf());
}

static void TestP2Add(CuTest* tc)
{
    TEST_OP2(add)
}

static void TestP2Sub(CuTest* tc)
{
    TEST_OP2(sub)
}

static void TestP2Mul(CuTest* tc)
{
    TEST_OP2(mul)
}

static void TestP2Div(CuTest* tc)
{
    TEST_OP2(div)
}

static void TestP2Is(CuTest* tc)
{
    Posit p = Posit(2, 0);
    Posit zero = p.zero();
    Posit one = p.one();
    Posit inf = p.inf();
    Posit mone = p.one().neg();

    CuAssertTrue(tc, zero.isZero());
    CuAssertTrue(tc, !one.isZero());
    CuAssertTrue(tc, !inf.isZero());
    CuAssertTrue(tc, !mone.isZero());

    CuAssertTrue(tc, !zero.isOne());
    CuAssertTrue(tc, one.isOne());
    CuAssertTrue(tc, !inf.isOne());
    CuAssertTrue(tc, mone.isOne());

    CuAssertTrue(tc, !zero.isInf());
    CuAssertTrue(tc, !one.isInf());
    CuAssertTrue(tc, inf.isInf());
    CuAssertTrue(tc, !mone.isInf());

    CuAssertTrue(tc, !zero.isNeg());
    CuAssertTrue(tc, !one.isNeg());
    CuAssertTrue(tc, !inf.isNeg());
    CuAssertTrue(tc, mone.isNeg());

    CuAssertTrue(tc, !zero.isNan());
    CuAssertTrue(tc, !one.isNan());
    CuAssertTrue(tc, !inf.isNan());
    CuAssertTrue(tc, !mone.isNan());
}

CuSuite* TestP2GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestP2Zero);
	SUITE_ADD_TEST(suite, TestP2Add);
	SUITE_ADD_TEST(suite, TestP2Sub);
	SUITE_ADD_TEST(suite, TestP2Mul);
	SUITE_ADD_TEST(suite, TestP2Div);
	SUITE_ADD_TEST(suite, TestP2Is);

	return suite;
}
