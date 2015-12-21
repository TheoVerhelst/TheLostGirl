#include <boost/test/unit_test.hpp>
#include <TheLostGirl/LangManager.h>

struct LangManagerTestsFixture
{
	LangManagerTestsFixture()
	{
	}

	~LangManagerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(LangManagerTests, LangManagerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
