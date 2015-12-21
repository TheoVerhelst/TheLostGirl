#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/ScriptsSystem.h>

struct ScriptsSystemTestsFixture
{
	ScriptsSystemTestsFixture()
	{
	}

	~ScriptsSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ScriptsSystemTests, ScriptsSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
