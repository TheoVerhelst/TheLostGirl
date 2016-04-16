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
	BOOST_CHECK(JsonHelper::isEqual(expectedValue, JsonHelper::loadFromFile(testingFilename)));
}

BOOST_AUTO_TEST_CASE(saveToFileTests)
{
	const std::string savedFile{"testsResources/testingJsonSave.json"};
	const Json::Value value{JsonHelper::loadFromFile(testingFilename)};
	JsonHelper::saveToFile(value, savedFile);
	BOOST_CHECK(JsonHelper::isEqual(JsonHelper::loadFromFile(savedFile), value));
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
	BOOST_CHECK(JsonHelper::isEqual(expectedValue, left));
	left = 4;
	BOOST_CHECK_THROW(JsonHelper::merge(left, right), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(isSubsetTest0)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["d"] = 5;//This element is not in right
	left["c"]["string"] = "s";
	left["c"]["array"][0] = "2";
	left["c"]["array"][1] = "3";
	right["a"] = 3;
	right["b"] = 4;
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	BOOST_CHECK(not JsonHelper::isSubset(left, right));
}

BOOST_AUTO_TEST_CASE(isSubsetTest1)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["b"] = 4.30000000000000002;
	left["d"] = "d";
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "2";
	right["a"] = 3;
	right["b"] = 4.30000000000000001;//Close enough, they should compare equal
	right["d"] = "d";
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	right["c"]["string"] = "s";
	BOOST_CHECK(JsonHelper::isSubset(left, right));
}

BOOST_AUTO_TEST_CASE(isSubsetTest2)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["a"] = 3;
	left["b"] = 4;
	left["c"]["string"] = "s";
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "3";//This element is not in right
	right["a"] = 3;
	right["b"] = 4;
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	right["c"]["string"] = "s";
	BOOST_CHECK(not JsonHelper::isSubset(left, right));
}

BOOST_AUTO_TEST_CASE(isSubsetTest3)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["a"] = 3;
	left["b"] = 5;//This element is different from the corresponding one in right
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "2";
	right["a"] = 3;
	right["b"] = 4;
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	right["c"]["string"] = "s";
	BOOST_CHECK(not JsonHelper::isSubset(left, right));
}

BOOST_AUTO_TEST_CASE(isEqualTest0)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["a"] = 3;
	left["b"] = 4.30000000000000002;
	left["d"] = "d";
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "1";
	left["c"]["array"][2] = "2";
	left["c"]["string"] = "s";
	right["a"] = 3;
	right["b"] = 4.30000000000000001;//Close enough, they should compare equal
	right["d"] = "d";
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	right["c"]["string"] = "s";
	BOOST_CHECK(JsonHelper::isEqual(left, right));
}

BOOST_AUTO_TEST_CASE(isEqualTest1)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["a"] = 3;
	left["b"] = 4.31;
	left["d"] = "d";
	right["a"] = 3;
	right["b"] = 4.32;//This element differs
	right["d"] = "d";
	BOOST_CHECK(not JsonHelper::isEqual(left, right));
}

BOOST_AUTO_TEST_CASE(isEqualTest2)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["array"][0] = "0";
	left["array"][1] = "1";
	right["array"][0] = "0";//Not the same length
	BOOST_CHECK(not JsonHelper::isEqual(left, right));
}

BOOST_AUTO_TEST_CASE(isEqualTest3)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["array"][0] = "0";
	left["array"][1] = "1";
	right["array"][0] = "2";//This element differs
	BOOST_CHECK(not JsonHelper::isEqual(left, right));
}

BOOST_AUTO_TEST_CASE(isEqualTest4)
{
	Json::Value left{Json::objectValue}, right{Json::realValue};//Different types
	BOOST_CHECK(not JsonHelper::isEqual(left, right));
}

BOOST_AUTO_TEST_CASE(substractTests)
{
	Json::Value left{Json::objectValue}, right{Json::objectValue};
	left["b"] = 4.30000000000000002;
	left["d"] = "d";
	left["c"]["array"][0] = "0";
	left["c"]["array"][1] = "2";
	right["a"] = 3;
	right["b"] = 4.30000000000000001;//Close enough, they should compare equal
	right["d"] = "d";
	right["c"]["array"][0] = "0";
	right["c"]["array"][1] = "1";
	right["c"]["array"][2] = "2";
	right["c"]["string"] = "s";
	BOOST_REQUIRE(JsonHelper::isSubset(left, right));
	JsonHelper::substract(right, left);
	Json::Value expectedValue;
	expectedValue["a"] = 3;
	expectedValue["c"]["array"][0] = "1";
	expectedValue["c"]["string"] = "s";
	BOOST_CHECK(JsonHelper::isEqual(expectedValue, right));
}

BOOST_AUTO_TEST_SUITE_END()
