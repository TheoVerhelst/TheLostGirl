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
	Json::Value expectedValue;
	Json::Reader reader;
	std::ifstream fileStream(testingFilename);
	const bool result{reader.parse(fileStream, expectedValue)};
	const std::string errorMessages{reader.getFormattedErrorMessages()};
	BOOST_REQUIRE_MESSAGE(result, errorMessages);
	BOOST_CHECK_EQUAL(expectedValue, JsonHelper::loadFromFile(testingFilename));
}

BOOST_AUTO_TEST_CASE(saveToFileTests)
{
	const std::string savedFile{"testsResources/testingJsonSave.json"};
	const Json::Value value{JsonHelper::loadFromFile(testingFilename)};
	JsonHelper::saveToFile(value, savedFile);
	BOOST_CHECK_EQUAL(JsonHelper::loadFromFile(savedFile), value);
}

BOOST_AUTO_TEST_CASE(mergeTests)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["a"] = 3;
	left["b"] = 4;
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "1";
	left["c"]["array"][2] = "2";
	right["d"] = 5;
	right["e"] = 6;
	right["b"] = 7;
	right["c"]["string"] = "s";
	right["c"]["array"][0] = "2";
	right["c"]["array"][1] = "3";
	JsonHelper::merge(left, right);
	Json::Value expectedValue;
	expectedValue["a"] = 3;
	expectedValue["b"] = 7;
	expectedValue["d"] = 5;
	expectedValue["e"] = 6;
	expectedValue["c"]["array"][0] = "0";
	expectedValue["c"]["array"][1] = "1";
	expectedValue["c"]["array"][2] = "2";
	expectedValue["c"]["array"][3] = "2";
	expectedValue["c"]["array"][4] = "3";
	expectedValue["c"]["string"] = "s";
	BOOST_CHECK_EQUAL(expectedValue, left);
}

BOOST_AUTO_TEST_SUITE_END()
