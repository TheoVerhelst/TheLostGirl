#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/AnimationsSystem.h>

struct AnimationsSystemTestsFixture
{
	AnimationsSystemTestsFixture()
	{
	}

	~AnimationsSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(AnimationsSystemTests, AnimationsSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
