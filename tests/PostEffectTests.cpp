#include <boost/test/unit_test.hpp>
#include <TheLostGirl/PostEffect.h>

struct PostEffectTestsFixture
{
	PostEffectTestsFixture()
	{
	}

	~PostEffectTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(PostEffectTests, PostEffectTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
