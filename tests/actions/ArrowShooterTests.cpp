#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/ArrowShooter.h>

struct ArrowShooterTestsFixture
{
	ArrowShooterTestsFixture()
	{
	}

	~ArrowShooterTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ArrowShooterTests, ArrowShooterTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
