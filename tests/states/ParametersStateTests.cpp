#include <boost/test/unit_test.hpp>
#include <TheLostGirl/states/ParametersState.h>

struct ParametersStateTestsFixture
{
	ParametersStateTestsFixture()
	{
	}

	~ParametersStateTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ParametersStateTests, ParametersStateTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
