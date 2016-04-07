#include <boost/test/unit_test.hpp>
#include <tests/CommandTests.hpp>

CommandTestsFixture::CommandTestsFixture():
	entityManager{eventManager},
	entity{entityManager.create()},
	category{Category::Player},
	targetSpecific{entity, [](entityx::Entity){}},
	targetNotSpecific{category, [](entityx::Entity){}}
{
}

CommandTestsFixture::~CommandTestsFixture()
{
}

BOOST_FIXTURE_TEST_SUITE(CommandTests, CommandTestsFixture)

	BOOST_AUTO_TEST_CASE(getters)
	{
		BOOST_REQUIRE(targetSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetSpecific.getEntity(), entity);

		BOOST_REQUIRE(not targetNotSpecific.isTargetSpecific());
		BOOST_CHECK(targetNotSpecific.getCategory() == category);
	}

	BOOST_AUTO_TEST_CASE(setters)
	{
		targetSpecific.setTarget(category);
		BOOST_CHECK(not targetSpecific.isTargetSpecific());
		BOOST_CHECK(targetSpecific.getCategory() == category);

		targetNotSpecific.setTarget(entity);
		BOOST_CHECK(targetNotSpecific.isTargetSpecific());
		BOOST_CHECK_EQUAL(targetNotSpecific.getEntity(), entity);
	}

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
