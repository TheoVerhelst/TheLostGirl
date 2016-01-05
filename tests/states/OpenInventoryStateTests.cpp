#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/OpenInventoryState.hpp>

struct OpenInventoryStateTestsFixture
{
	OpenInventoryStateTestsFixture()
	{
	}

	~OpenInventoryStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(OpenInventoryStateTests, OpenInventoryStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
