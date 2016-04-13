#include <fstream>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <tests/LangManagerTests.hpp>


LangManagerTestsFixture::LangManagerTestsFixture():
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

LangManagerTestsFixture::~LangManagerTestsFixture()
{
	delete m_parameters;
}

inline std::size_t LangManagerTestsFixture::getFileLineNumber(const std::string& filename)
{
	std::ifstream fileStream{filename};
	const auto res(std::count(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>(), '\n'));
	return static_cast<std::size_t>(res);
}

BOOST_FIXTURE_TEST_SUITE(LangManagerTests, LangManagerTestsFixture)

BOOST_AUTO_TEST_CASE(trTests)
{
	//We will open the default lang file and a known translation file, register
	//all their lines and make sure that the lang manager has the same
	//translation as we just got.

	std::map<std::string, std::wstring> entries;
	const std::string testedLang{"FR"};
	const std::string defaultFilename{m_parameters->resourcesPath + "lang/" + langs.getDefaultLang()};
	const std::string translationFilename{m_parameters->resourcesPath + "lang/" + testedLang};

	std::ifstream defaultFileStream{defaultFilename};
	std::wifstream translationFileStream;
	//Things to handle wide encoding
	translationFileStream.imbue(std::locale(""));
	translationFileStream.open(translationFilename);

	//Check if the langs files are loaded
	BOOST_REQUIRE(defaultFileStream.is_open());
	BOOST_REQUIRE(translationFileStream.is_open());

	std::string sourceLine;
	std::wstring translatedLine;
	while(not defaultFileStream.eof() and not translationFileStream.eof())
	{
		getline(defaultFileStream, sourceLine);
		getline(translationFileStream, translatedLine);
		entries.emplace(sourceLine, translatedLine);
	}

	BOOST_CHECK_MESSAGE(defaultFileStream.eof() == translationFileStream.eof(),
			"It seems that lang files are not consistent (" + defaultFilename + " and " + translationFilename + ").");
	defaultFileStream.close();
	translationFileStream.close();
	langs.setLang(testedLang);//Set the tested lang to the lang manager

	//Test normal strings
	for(auto& pair : entries)
		BOOST_CHECK(langs.tr(pair.first) == pair.second);

	//Test a wrong string
	std::string unlikelyString{"*ù%$_This is very unlikely to find this string in lang file-$°)@à"};
	std::wstring wUnlikelystring{unlikelyString.begin(), unlikelyString.end()};
	logStream(std::cerr);
	//Check that it returns the original string in wide encoding
	BOOST_CHECK(langs.tr(unlikelyString) == wUnlikelystring);
	const std::string errorMessage{getStreamLog()};
	//Check that the error message contains the original string
	BOOST_CHECK(errorMessage.find(unlikelyString) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(langsTests)
{
	//Test actual languages, not testing files. We only ensure that all files
	//have the same length as the default lang file.
	m_parameters->resourcesPath = "resources/";
	const std::string defaultFilename{m_parameters->resourcesPath + "lang/" + langs.getDefaultLang()};
	std::string translationFilename;
	const std::size_t sourceLineNumber{getFileLineNumber(defaultFilename)};

	//Check that all lang files are complete
	for(auto& availableLang : LangManager::getAvailableLangs())
	{
		translationFilename = m_parameters->resourcesPath + "lang/" + availableLang;
		BOOST_CHECK_MESSAGE(sourceLineNumber == getFileLineNumber(translationFilename),
				"It seems that lang files are not consistent (" + defaultFilename + " and " + translationFilename + ").");
	}
}

BOOST_AUTO_TEST_CASE(setLangTests)
{
	const std::string defaultLang{langs.getDefaultLang()}, testedLang{"FR"};
	langs.setLang(testedLang);
	BOOST_CHECK(langs.getLang() == testedLang);
	const std::string notExistingLang{"Not existing lang"};
	logStream(std::cerr);
	langs.setLang(notExistingLang);
	const std::string errorMessage{getStreamLog()};
	//Check that the error message contains the lang name
	BOOST_CHECK(errorMessage.find(notExistingLang) != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
