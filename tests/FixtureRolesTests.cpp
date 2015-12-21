#include <boost/test/unit_test.hpp>
#include <TheLostGirl/FixtureRoles.h>

struct FixtureRolesTestsFixture
{
	FixtureRolesTestsFixture()
	{
	}

	~FixtureRolesTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(FixtureRolesTests, FixtureRolesTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
