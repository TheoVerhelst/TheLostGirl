#include <boost/test/unit_test.hpp>
#include <tests/CommandTests.hpp>

CommandTestsFixture::CommandTestsFixture():
	entityManager{eventManager},
	entity{entityManager.create()},
	otehrEntity{entityManager.create()},
	category{Category::Player},
	otherCategory{Category::Scene},
	targetSpecific{entity, action},
	targetNotSpecific{category, action}
{
}

CommandTestsFixture::~CommandTestsFixture()
{
}

void action(entityx::Entity entity)
{
}

BOOST_FIXTURE_TEST_SUITE(CommandTests, CommandTestsFixture)

	BOOST_AUTO_TEST_CASE(getters)
	{
		BOOST_CHECK_THROW(targetSpecific.getCategory(), std::logic_error);
		BOOST_REQUIRE(targetSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetSpecific.getEntity(), entity);

		BOOST_CHECK_THROW(targetSpecific.getEntity(), std::logic_error);
		BOOST_REQUIRE(not targetNotSpecific.isTargetSpecific());
		BOOST_CHECK(targetNotSpecific.getCategory() == category);

		// Test action
		const auto targetSpecificAction(targetSpecific.target<void(*)(entityx::Entity)>());
		const auto targetNotSpecificAction(targetNotSpecific.target<void(*)(entityx::Entity)>());
		BOOST_CHECK_EQUAL(targetSpecificAction, targetNotSpecificAction);
	}

	BOOST_AUTO_TEST_CASE(setters)
	{
		targetSpecific.setTarget(otherEntity);
		BOOST_CHECK(targetSpecific.isTargetSpecific());
		BOOST_CHECK(targetSpecific.getEntity() == otherEntity);

		targetSpecific.setTarget(category);
		BOOST_CHECK(not targetSpecific.isTargetSpecific());
		BOOST_CHECK(targetSpecific.getCategory() == category);

		targetNotSpecific.setTarget(otherCategory);
		BOOST_CHECK(not targetNotSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetNotSpecific.getCategory(), otherCategory);

		targetNotSpecific.setTarget(entity);
		BOOST_CHECK(targetNotSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetNotSpecific.getEntity(), entity);

		// Test action
		targetSpecific.setAction(otherAction);
		BOOST_CHECK_EQUAL(targetSpecific.target<void(*)(entityx::Entity)>(), otherAction);
	}

	BOOST_AUTO_TEST_CASE(copyConstructor)
	{
		{
			Command command(targetSpecific);
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}

		{
			Command command(targetNotSpecific);
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}
	}

	BOOST_AUTO_TEST_CASE(copyAssignmentOperator)
	{
		{
			Command command(targetNotSpecific);
			command = targetSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}

		{
			Command command(targetSpecific);
			command = targetNotSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}

		{
			Command command(otherEntity, otherAction);
			command = targetSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), true);
			BOOST_CHECK_EQUAL(command.getEntity(), entity);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}

		{
			Command command(otherCategory, otherAction);
			command = targetNotSpecific;
			BOOST_REQUIRE_EQUAL(command.isTargetSpecific(), false);
			BOOST_CHECK(command.getCategory() == category);
			BOOST_CHECK_EQUAL(command.target<void(*)(entityx::Entity)>(), action);
		}
	}

BOOST_AUTO_TEST_SUITE_END()
