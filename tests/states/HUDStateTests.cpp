#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/HUDState.h>

struct HUDStateTestsFixture
{
	HUDStateTestsFixture()
	{
	}

	~HUDStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(HUDStateTests, HUDStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()