#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/RenderSystem.h>

struct RenderSystemTestsFixture
{
	RenderSystemTestsFixture()
	{
	}

	~RenderSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(RenderSystemTests, RenderSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()