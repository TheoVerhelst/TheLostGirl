#include <boost/test/unit_test.hpp>
#include <TheLostGirl/scripts/Interpreter.hpp>

struct InterpreterTestsFixture
{
	InterpreterTestsFixture()
	{
	}

	~InterpreterTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(InterpreterTests, InterpreterTestsFixture)
 
BOOST_AUTO_TEST_CASE(MyMethod)
{
    BOOST_CHECK(1 + 1 == 2);
}
 
BOOST_AUTO_TEST_SUITE_END()
