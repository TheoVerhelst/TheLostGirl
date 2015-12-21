#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/Death.h>

struct DeathTestsFixture
{
	DeathTestsFixture()
	{
	}

	~DeathTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(DeathTests, DeathTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
