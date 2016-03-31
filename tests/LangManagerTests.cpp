#include <fstream>
#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/LangManager.hpp>

struct LangManagerTestsFixture
{
	LangManager langs;
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) Type* m_attribute{nullptr};
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
	ContextImpl context;

	/// Constructor.
	LangManagerTestsFixture():
		m_parameters{new Parameters()},
		context
		{
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) *m_attribute,
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
		}
	{
		m_parameters->resourcesPath = "testsResources/";
	}

	/// Destructor.
	~LangManagerTestsFixture()
	{
		delete m_parameters;
	}
};

BOOST_FIXTURE_TEST_SUITE(LangManagerTests, LangManagerTestsFixture)

BOOST_AUTO_TEST_CASE(langsTests)
{
	std::map<std::string, std::wstring> entries;
	const std::string sourceFileName{m_parameters->resourcesPath + "lang/" + langs.getDefaultLang()};
	std::string translationFileName;
	std::ifstream sourceFileStream;
	std::wifstream translationFileStream;

	//Check if all translation files are complete
	for(auto& availableLang : LangManager::getAvailableLangs())
	{
		translationFileName = m_parameters->resourcesPath + "lang/" + availableLang;
		entries.clear();
		sourceFileStream.open(sourceFileName);
		//Things to handle wide encoding
		translationFileStream.imbue(std::locale(""));
		translationFileStream.open(translationFileName);

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

		BOOST_REQUIRE_MESSAGE(sourceFileStream.eof() == translationFileStream.eof(),
				"It seems that lang files are not consistent (" + sourceFileName + " and " + translationFileName + ").");
		sourceFileStream.close();
		translationFileStream.close();

		langs.setLang(availableLang);

		for(auto& pair : entries)
			BOOST_CHECK(langs.tr(pair.first) == pair.second);
	}
}

BOOST_AUTO_TEST_CASE(setLangTests)
{
	const std::set<std::string> availableLangs{LangManager::getAvailableLangs()};
	if(availableLangs.size() > 1)
	{
		const std::string defaultLang{langs.getDefaultLang()};
		BOOST_CHECK(std::find(availableLangs.begin(), availableLangs.end(), defaultLang) != availableLangs.end());
		auto it = std::find_if(availableLangs.begin(), availableLangs.end(), [&defaultLang](const std::string& lang)
		{
			return defaultLang != lang;
		});
		BOOST_REQUIRE(it != availableLangs.end());
		const std::string newLang{*it};
		langs.setLang(newLang);
		BOOST_CHECK(langs.getLang() == newLang);
	}
	BOOST_WARN_MESSAGE(availableLangs.size() > 1, "No second lang was found for testing LangManager::setLang");
}

BOOST_AUTO_TEST_SUITE_END()
