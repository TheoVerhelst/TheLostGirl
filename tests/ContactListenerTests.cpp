#include <boost/test/unit_test.hpp>
#include <TheLostGirl/ContactListener.h>

struct ContactListenerTestsFixture
{
	ContactListenerTestsFixture()
	{
	}

	~ContactListenerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(ContactListenerTests, ContactListenerTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
