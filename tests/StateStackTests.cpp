#include <boost/test/unit_test.hpp>
#include <TheLostGirl/StateStack.h>

struct StateStackTestsFixture
{
	StateStackTestsFixture()
	{
	}

	~StateStackTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(StateStackTests, StateStackTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
