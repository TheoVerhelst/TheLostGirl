#include <boost/test/unit_test.hpp>
#include <TheLostGirl/scripts/scriptsFunctions.h>

struct scriptsFunctionsTestsFixture
{
	scriptsFunctionsTestsFixture()
	{
	}

	~scriptsFunctionsTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(scriptsFunctionsTests, scriptsFunctionsTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()