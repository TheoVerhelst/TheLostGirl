#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/LoadingState.h>

struct LoadingStateTestsFixture
{
	LoadingStateTestsFixture()
	{
	}

	~LoadingStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(LoadingStateTests, LoadingStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
