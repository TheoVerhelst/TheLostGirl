#include <boost/test/unit_test.hpp>
#include <TheLostGirl/scripts/Tree.h>

struct TreeTestsFixture
{
	TreeTestsFixture()
	{
	}

	~TreeTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(TreeTests, TreeTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
