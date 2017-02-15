#include <cassert>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "CuTest.h"
#include "posit.h"

void TestP2Zero(CuTest* tc)
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
    CuAssertTrue(tc, rec.eq(inf));
}

CuSuite* TestP2GetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestP2Zero);

	return suite;
}
