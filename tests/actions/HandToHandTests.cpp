#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/HandToHand.h>

struct HandToHandTestsFixture
{
	HandToHandTestsFixture()
	{
	}

	~HandToHandTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(HandToHandTests, HandToHandTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
