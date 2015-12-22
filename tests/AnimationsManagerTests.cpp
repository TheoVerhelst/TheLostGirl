#include <boost/test/unit_test.hpp>
#include <TheLostGirl/AnimationsManager.h>

class MockAnimation
{
	public:
		void setProgress(float)
		{
		}
};

struct AnimationsManagerTestsFixture
{
	AnimationsManager<MockAnimation> manager;

	AnimationsManagerTestsFixture()
	{
		manager.addAnimation("a", MockAnimation(), 0, sf::seconds(1.f), false, false);
	}

	~AnimationsManagerTestsFixture()
	{
	}

};

BOOST_FIXTURE_TEST_SUITE(AnimationsManagerTests, AnimationsManagerTestsFixture)

	BOOST_AUTO_TEST_CASE(getters)
	{
		manager.isRegistred("a");
		BOOST_CHECK(manager.isPaused("a"));
		BOOST_CHECK(not manager.isActive("a"));
		BOOST_CHECK_EQUAL(manager.getProgress("a"), 0.f);
	}

	BOOST_AUTO_TEST_CASE(setProgress)
	{
		manager.setProgress("a", 0.5f);
		BOOST_CHECK_EQUAL(manager.getProgress("a"), 0.5f);
	}

	BOOST_AUTO_TEST_CASE(remove)
	{
		manager.removeAnimation("a");
		BOOST_CHECK(not manager.isRegistred("a"));
	}

	BOOST_AUTO_TEST_CASE(play)
	{
		manager.setProgress("a", 0.5f);
		manager.play("a");
		BOOST_CHECK(not manager.isPaused("a"));
		BOOST_CHECK(manager.isActive("a"));
		BOOST_CHECK_EQUAL(manager.getProgress("a"), 0.5f);

		manager.pause("a");
		BOOST_CHECK(manager.isPaused("a"));
		BOOST_CHECK(manager.isActive("a"));
		BOOST_CHECK_EQUAL(manager.getProgress("a"), 0.5f);

		manager.stop("a");
		BOOST_CHECK(manager.isPaused("a"));
		BOOST_CHECK(not manager.isActive("a"));
		BOOST_CHECK_EQUAL(manager.getProgress("a"), 0.f);
	}

BOOST_AUTO_TEST_SUITE_END()
