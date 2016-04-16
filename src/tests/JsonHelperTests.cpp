#include <fstream>
#include <boost/test/unit_test.hpp>
#include <TheLostGirl/JsonHelper.hpp>
#include <tests/JsonHelperTests.hpp>

JsonHelperTestsFixture::JsonHelperTestsFixture():
	testingFilename{"testsResources/testingValue.json"}
{
}

BOOST_FIXTURE_TEST_SUITE(JsonHelperTests, JsonHelperTestsFixture)

BOOST_AUTO_TEST_CASE(loadFromFileTests)
{
	Json::Value value;
	Json::Reader reader;
	std::ifstream fileStream(testingFilename);
	const bool result{reader.parse(fileStream, value)};
	const std::string errorMessages{reader.getFormattedErrorMessages()};
	BOOST_REQUIRE_MESSAGE(result, errorMessages);
	BOOST_CHECK(value == JsonHelper::loadFromFile(testingFilename));
}

BOOST_AUTO_TEST_SUITE_END()
