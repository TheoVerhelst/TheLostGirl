#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>

struct PendingChangesSystemTestsFixture
{
	PendingChangesSystemTestsFixture()
	{
	}

	~PendingChangesSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(PendingChangesSystemTests, PendingChangesSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
