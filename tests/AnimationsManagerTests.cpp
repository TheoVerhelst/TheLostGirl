#include <boost/test/unit_test.hpp>
#include <TheLostGirl/AnimationsManager.h>

//TODO Debug this test case

class MockAnimation
{
	public:
		void setProgress(float progress)
		{
		}
};

struct TestFixture
{
	TestFixture()
	{
	}

	~TestFixture()
	{
	}

};

BOOST_FIXTURE_TEST_SUITE(AnimationsManagerTests, TestFixture)

	BOOST_AUTO_TEST_CASE(getters)
	{
//		manager.isRegistred("a");
	//	BOOST_TEST(manager.isPaused("a"));
	//	BOOST_TEST(not manager.isActive("a"));
	//	BOOST_TEST(manager.getProgress("a") == 0.f);
	}

	BOOST_AUTO_TEST_CASE(setProgress)
	{
	//	manager.setProgress("a", 0.5f);
	//	BOOST_TEST(manager.getProgress("a"), 0.5f);
	}

	BOOST_AUTO_TEST_CASE(remove)
	{
	//	manager.removeAnimation("a");
	//	BOOST_TEST(not manager.isRegistred("a"));
	}

	BOOST_AUTO_TEST_CASE(play)
	{
	//	manager.setProgress("a", 0.5f);
	//	manager.play("a");
	//	BOOST_TEST(not manager.isPaused("a"));
	//	BOOST_TEST(manager.isActive("a"));
	//	BOOST_TEST(manager.getProgress("a"), 0.5f);

	//	manager.pause("a");
	//	BOOST_TEST(manager.isPaused("a"));
	//	BOOST_TEST(manager.isActive("a"));
	//	BOOST_TEST(manager.getProgress("a"), 0.5f);
	//
	//	manager.stop("a");
	//	BOOST_TEST(manager.isPaused("a"));
	//	BOOST_TEST(not manager.isActive("a"));
	//	BOOST_TEST(manager.getProgress("a"), 0.f);
	}

BOOST_AUTO_TEST_SUITE_END()
