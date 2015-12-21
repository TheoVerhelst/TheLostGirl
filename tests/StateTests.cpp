#include <boost/test/unit_test.hpp>
#include <TheLostGirl/State.h>

struct StateTestsFixture
{
	StateTestsFixture()
	{
	}

	~StateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(StateTests, StateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
