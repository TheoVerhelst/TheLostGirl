#include <boost/test/unit_test.hpp>
#include <tests/CommandTests.hpp>

CommandTestsFixture::CommandTestsFixture():
	entityManager{eventManager},
	entity{entityManager.create()},
	otherEntity{entityManager.create()},
	category{Category::Player},
	otherCategory{Category::Scene},
	targetSpecific{entity, action},
	targetNotSpecific{category, action}
{
}

CommandTestsFixture::~CommandTestsFixture()
{
}

void CommandTestsFixture::checkAction(Command& command, void (*action)(entityx::Entity))
{
	BOOST_CHECK_EQUAL(*command.getAction().target<void(*)(entityx::Entity)>(), action);
}

void action(entityx::Entity)
{
}

void otherAction(entityx::Entity)
{
}

BOOST_FIXTURE_TEST_SUITE(CommandTests, CommandTestsFixture)

	BOOST_AUTO_TEST_CASE(getters)
	{
		BOOST_CHECK_THROW(targetSpecific.getCategory(), std::logic_error);
		BOOST_REQUIRE(targetSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetSpecific.getEntity(), entity);

		BOOST_CHECK_THROW(targetNotSpecific.getEntity(), std::logic_error);
		BOOST_REQUIRE(not targetNotSpecific.isTargetSpecific());
		BOOST_CHECK(targetNotSpecific.getCategory() == category);

		// Test action
		checkAction(targetSpecific, action);
		checkAction(targetNotSpecific, action);
	}

	BOOST_AUTO_TEST_CASE(setters)
	{
		targetSpecific.setTarget(otherEntity);
		BOOST_CHECK(targetSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetSpecific.getEntity(), otherEntity);

		targetSpecific.setTarget(category);
		BOOST_CHECK(not targetSpecific.isTargetSpecific());
		BOOST_CHECK(targetSpecific.getCategory() == category);

		targetNotSpecific.setTarget(otherCategory);
		BOOST_CHECK(not targetNotSpecific.isTargetSpecific());
		BOOST_CHECK(targetNotSpecific.getCategory() == otherCategory);

		targetNotSpecific.setTarget(entity);
		BOOST_CHECK(targetNotSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetNotSpecific.getEntity(), entity);

		// Test action
		targetSpecific.setAction(otherAction);
		checkAction(targetSpecific, otherAction);
	}

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		{
			Command command(targetSpecific);
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			checkAction(command, action);
		}

		{
			Command command(targetNotSpecific);
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			checkAction(command, action);
		}
	}

	BOOST_AUTO_TEST_CASE(copyAssignmentOperator)
	{
		{
			Command command(targetNotSpecific);
			command = targetSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			checkAction(command, action);
		}

		{
			Command command(targetSpecific);
			command = targetNotSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			checkAction(command, action);
		}

		{
			Command command(otherEntity, otherAction);
			command = targetSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			checkAction(command, action);
		}

		{
			Command command(otherCategory, otherAction);
			command = targetNotSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			checkAction(command, action);
		}
	}

BOOST_AUTO_TEST_SUITE_END()
