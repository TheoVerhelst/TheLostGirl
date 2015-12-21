#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/GameState.h>

struct GameStateTestsFixture
{
	GameStateTestsFixture()
	{
	}

	~GameStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(GameStateTests, GameStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
