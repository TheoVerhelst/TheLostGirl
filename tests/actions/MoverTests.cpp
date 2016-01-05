#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/Mover.hpp>

struct MoverTestsFixture
{
	MoverTestsFixture()
	{
	}

	~MoverTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(MoverTests, MoverTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
