#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/InventoryOpener.h>

struct Data
{
	Data()
	{
	}

	~Data()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(InventoryOpener, Data)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
