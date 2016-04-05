#include <boost/test/unit_test.hpp>
#include <TheLostGirl/contactListeners/ArrowHitListener.hpp>

struct ArrowHitListenerTestsFixture
{
	ArrowHitListenerTestsFixture()
	{
	}

	~ArrowHitListenerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ArrowHitListenerTests, ArrowHitListenerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
