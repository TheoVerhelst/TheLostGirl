#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/PauseState.h>

struct PauseStateTestsFixture
{
	PauseStateTestsFixture()
	{
	}

	~PauseStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(PauseStateTests, PauseStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
