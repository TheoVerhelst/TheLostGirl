#include <boost/test/unit_test.hpp>
#include <TheLostGirl/contactListeners/FallingListener.hpp>

struct FallingListenerTestsFixture
{
	FallingListenerTestsFixture()
	{
	}

	~FallingListenerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(FallingListenerTests, FallingListenerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
