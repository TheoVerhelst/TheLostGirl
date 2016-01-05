#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/KeyConfigurationState.hpp>

struct KeyConfigurationStateTestsFixture
{
	KeyConfigurationStateTestsFixture()
	{
	}

	~KeyConfigurationStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(KeyConfigurationStateTests, KeyConfigurationStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
