#include <boost/test/unit_test.hpp>
#include <TheLostGirl/SkyAnimation.hpp>

struct SkyAnimationTestsFixture
{
	SkyAnimationTestsFixture()
	{
	}

	~SkyAnimationTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(SkyAnimationTests, SkyAnimationTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
