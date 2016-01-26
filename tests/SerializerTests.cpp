#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Serializer.hpp>

struct SerializerTestsFixture
{
	SerializerTestsFixture()
	{
	}

	~SerializerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(SerializerTests, SerializerTestsFixture)

BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}

BOOST_AUTO_TEST_SUITE_END()
