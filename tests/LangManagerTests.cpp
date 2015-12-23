#include <fstream>
#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/LangManager.h>

struct LangManagerTestsFixture
{
	LangManager langs;
	Parameters parameters;
	LangManagerTestsFixture()
	{
		Context::parameters = &parameters;
	}

	~LangManagerTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(LangManagerTests, LangManagerTestsFixture)

BOOST_AUTO_TEST_CASE(LangTests)
{
	std::map<std::string, std::wstring> entries;
	const std::string sourceFileName{parameters.resourcesPath + "lang/EN"};
	std::ifstream sourceFileStream;
	std::wifstream translationFileStream;

	//Check if all translation files are complete
	for(auto& availableLang : LangManager::getAvailableLangs())
	{
		entries.clear();
		sourceFileStream.open(sourceFileName);
		//Things to handle wide encoding
		translationFileStream.imbue(std::locale(""));
		translationFileStream.open(parameters.resourcesPath + "lang/" + availableLang);

		//Check if the langs files are loaded
		BOOST_REQUIRE(sourceFileStream.is_open());
		BOOST_REQUIRE(translationFileStream.is_open());

		std::string sourceLine;
		std::wstring translatedLine;
		while(not sourceFileStream.eof() and not translationFileStream.eof())
		{
			getline(sourceFileStream, sourceLine);
			getline(translationFileStream, translatedLine);
			entries.emplace(sourceLine, translatedLine);
		}

		BOOST_REQUIRE_MESSAGE(sourceFileStream.eof() == translationFileStream.eof(), "It seems that lang files are not consistent");
		sourceFileStream.close();
		translationFileStream.close();

		langs.setLang(availableLang);

		for(auto& pair : entries)
			BOOST_CHECK(langs.tr(pair.first) == pair.second);
	}
}

BOOST_AUTO_TEST_SUITE_END()
