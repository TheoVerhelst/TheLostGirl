#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/StatsSystem.h>

struct StatsSystemTestsFixture
{
	StatsSystemTestsFixture()
	{
	}

	~StatsSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(StatsSystemTests, StatsSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
