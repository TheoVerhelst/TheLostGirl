#include <boost/test/unit_test.hpp>
#include <entityx/Entity.h>
#include <TheLostGirl/Command.h>

struct CommandTestsFixture
{
	entityx::EventManager eventManager;
	entityx::EntityManager entityManager;
	entityx::Entity entity;
	unsigned int category;
	Command targetSpecific;
	Command targetNotSpecific;

	CommandTestsFixture():
		entityManager{eventManager},
		entity{entityManager.create()},
		category{2}
	{
		targetSpecific.action = [](entityx::Entity){};
		targetSpecific.targetIsSpecific = true;
		targetSpecific.entity = entity;

		targetNotSpecific.action = [](entityx::Entity){};
		targetNotSpecific.targetIsSpecific = false;
		targetNotSpecific.category = category;
	}

	~CommandTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(CommandTests, CommandTestsFixture)

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		Command command(targetSpecific);
		BOOST_CHECK_EQUAL(targetSpecific.entity, command.entity);
		BOOST_CHECK_EQUAL(targetSpecific.targetIsSpecific, command.targetIsSpecific);

		Command otherCommand(targetNotSpecific);
		BOOST_CHECK_EQUAL(targetNotSpecific.category, otherCommand.category);
		BOOST_CHECK_EQUAL(targetNotSpecific.targetIsSpecific, otherCommand.targetIsSpecific);
	}

BOOST_AUTO_TEST_SUITE_END()
