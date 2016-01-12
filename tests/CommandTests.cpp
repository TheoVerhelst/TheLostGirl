#include <boost/test/unit_test.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/Command.hpp>

struct CommandTestsFixture
{
	entityx::EventManager eventManager;
	entityx::EntityManager entityManager;
	entityx::Entity entity;
	FlagSet<Category> category;
	Command targetSpecific;
	Command targetNotSpecific;

	CommandTestsFixture():
		entityManager{eventManager},
		entity{entityManager.create()},
		category{Category::Player},
		targetSpecific{entity, [](entityx::Entity){}},
		targetNotSpecific{category, [](entityx::Entity){}}
	{
	}

	~CommandTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(CommandTests, CommandTestsFixture)

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		Command command(targetSpecific);
		BOOST_REQUIRE_EQUAL(targetSpecific.isTargetSpecific(), command.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetSpecific.getEntity(), command.getEntity());

		Command otherCommand(targetNotSpecific);
		BOOST_REQUIRE_EQUAL(targetNotSpecific.isTargetSpecific(), otherCommand.isTargetSpecific());
		BOOST_CHECK(targetNotSpecific.getCategory() == otherCommand.getCategory());
	}

BOOST_AUTO_TEST_SUITE_END()
