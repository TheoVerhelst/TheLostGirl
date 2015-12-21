#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/TimeSystem.h>

struct TimeSystemTestsFixture
{
	TimeSystemTestsFixture()
	{
	}

	~TimeSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(TimeSystemTests, TimeSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
