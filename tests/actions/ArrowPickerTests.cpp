#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/ArrowPicker.h>

struct TestFixture
{
	TestFixture()
	{
	}

	~TestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ArrowPickerTests, TestFixture)

BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}

BOOST_AUTO_TEST_SUITE_END()
