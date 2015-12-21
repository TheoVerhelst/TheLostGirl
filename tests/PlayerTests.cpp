#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Player.h>

struct PlayerTestsFixture
{
	PlayerTestsFixture()
	{
	}

	~PlayerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(PlayerTests, PlayerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
