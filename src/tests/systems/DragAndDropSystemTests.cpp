#include <boost/test/unit_test.hpp>
#include <TheLostGirl/systems/DragAndDropSystem.hpp>

struct DragAndDropSystemTestsFixture
{
	DragAndDropSystemTestsFixture()
	{
	}

	~DragAndDropSystemTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(DragAndDropSystemTests, DragAndDropSystemTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
