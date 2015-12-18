#include <boost/test/unit_test.hpp>
#include <TheLostGirl/scripts/ScriptError.h>

struct TestFixture
{
	TestFixture()
	{
	}

	~TestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ScriptErrorTests, TestFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
