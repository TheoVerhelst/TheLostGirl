#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/IntroState.h>

struct IntroStateTestsFixture
{
	IntroStateTestsFixture()
	{
	}

	~IntroStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(IntroStateTests, IntroStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()