#include <boost/test/unit_test.hpp>
#include <TheLostGirl/actions/CorpseSearcher.h>

struct CorpseSearcherTestsFixture
{
	CorpseSearcherTestsFixture()
	{
	}

	~CorpseSearcherTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(CorpseSearcherTests, CorpseSearcherTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
