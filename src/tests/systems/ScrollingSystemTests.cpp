#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/ScrollingSystem.hpp>

struct ScrollingSystemTestsFixture
{
	ScrollingSystemTestsFixture()
	{
	}

	~ScrollingSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ScrollingSystemTests, ScrollingSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
