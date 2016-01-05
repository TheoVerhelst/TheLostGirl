#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/Jumper.hpp>

struct JumperTestsFixture
{
	JumperTestsFixture()
	{
	}

	~JumperTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(JumperTests, JumperTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
