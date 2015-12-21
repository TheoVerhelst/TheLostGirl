#include <boost/test/unit_test.hpp>
#include <TheLostGirl/SpriteSheetAnimation.h>

struct SpriteSheetAnimationTestsFixture
{
	SpriteSheetAnimationTestsFixture()
	{
	}

	~SpriteSheetAnimationTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(SpriteSheetAnimationTests, SpriteSheetAnimationTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
