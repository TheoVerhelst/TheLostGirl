#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems.hpp>
#include <tests/PlayerTests.hpp>

PlayerTestsFixture::PlayerTestsFixture():
	entities{events},
	systems{entities, events}
{
	systems.add<PendingChangesSystem>();
	systems.add<DragAndDropSystem>();
}

PlayerTestsFixture::~PlayerTestsFixture()
{
}

BOOST_FIXTURE_TEST_SUITE(PlayerTests, PlayerTestsFixture)

//TODO Need to implement the command config file, in order to test key assignement in the player class

BOOST_AUTO_TEST_SUITE_END()
