#include <boost/test/unit_test.hpp>
#include <TheLostGirl/functions.h>

struct functionsTestsFixture
{
	functionsTestsFixture()
	{
	}

	~functionsTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(functionsTests, functionsTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
