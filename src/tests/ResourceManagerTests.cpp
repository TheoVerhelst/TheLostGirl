#include <boost/test/unit_test.hpp>
#include <TheLostGirl/ResourceManager.hpp>

struct ResourceManagerTestsFixture
{
	ResourceManagerTestsFixture()
	{
	}

	~ResourceManagerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ResourceManagerTests, ResourceManagerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
