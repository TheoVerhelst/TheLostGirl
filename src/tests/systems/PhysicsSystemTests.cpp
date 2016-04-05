#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/PhysicsSystem.hpp>

struct PhysicsSystemTestsFixture
{
	PhysicsSystemTestsFixture()
	{
	}

	~PhysicsSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(PhysicsSystemTests, PhysicsSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
