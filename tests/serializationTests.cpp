#include <boost/test/unit_test.hpp>
#include <TheLostGirl/serialization.h>

struct serializationTestsFixture
{
	serializationTestsFixture()
	{
	}

	~serializationTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(serializationTests, serializationTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
