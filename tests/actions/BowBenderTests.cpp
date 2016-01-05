#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/BowBender.hpp>

struct BowBenderTestsFixture
{
	BowBenderTestsFixture()
	{
	}

	~BowBenderTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(BowBenderTests, BowBenderTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
