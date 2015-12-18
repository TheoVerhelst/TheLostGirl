#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/CorpseSearcher.h>

struct Data
{
	Data()
	{
	}

	~Data()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(CorpseSearcher, Data)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
