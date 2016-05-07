#include <fstream>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <tests/LangManagerTests.hpp>

namespace fs = std::experimental::filesystem;

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

inline std::size_t LangManagerTestsFixture::getFileLineNumber(const fs::path& filePath)
{
	std::ifstream fileStream{filePath.string()};
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
	const fs::path testedLang{"FR"};
	const fs::path resourcesPath{m_parameters->resourcesPath / fs::path{"lang/"}};
	const fs::path sourceDirectoryPath{resourcesPath / langs.getDefaultLang()};
	const fs::path translationDirectoryPath{resourcesPath / testedLang};

	BOOST_REQUIRE(fs::is_directory(sourceDirectoryPath));
	BOOST_REQUIRE(fs::is_directory(translationDirectoryPath));
	
	for(auto& directoryEntry : fs::directory_iterator(sourceDirectoryPath))
	{
		//Get the path of both source and translation files
		const fs::path sourceFilePath{directoryEntry.path()};
		const fs::path translationFilePath{translationDirectoryPath / sourceFilePath.filename()};
		
		//Check that the source file and the translation file are regular files
		if(not fs::is_regular_file(sourceFilePath) or not fs::is_regular_file(translationFilePath))
		{
			BOOST_WARN_MESSAGE(false, "The source file and the translation file aren't regular files "
					"(" + sourceFilePath.string() + " and " + translationFilePath.string() + ").");
			continue;
		}
		
		std::ifstream sourceFileStream{sourceFilePath.string()};
		std::wifstream translationFileStream;

		//Things to handle wide encoding
		translationFileStream.imbue(std::locale(""));
		translationFileStream.open(translationFilePath.string());

		//Check if the langs files are loaded
		BOOST_REQUIRE(sourceFileStream.is_open());
		BOOST_REQUIRE(translationFileStream.is_open());
		
		std::string sourceLine;
		std::wstring translatedLine;
		while(not sourceFileStream.eof() and not translationFileStream.eof())
		{
			getline(sourceFileStream, sourceLine);//fill line with the next line of filestream
			getline(translationFileStream, translatedLine);//fill line with the next line of filestream
			entries.emplace(sourceLine, translatedLine);
		}

		BOOST_CHECK_MESSAGE(sourceFileStream.eof() == translationFileStream.eof(),
				"It seems that lang files are not consistent "
				"(" + sourceFilePath.string() + " and " + translationFilePath.string() + ").");
	}
	
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
	const fs::path resourcesPath{m_parameters->resourcesPath / fs::path{"lang/"}};
	const fs::path sourceDirectoryPath{resourcesPath / langs.getDefaultLang()};
	
	for(auto& directoryEntry : fs::directory_iterator(sourceDirectoryPath))
	{
		//Get the path of source file
		const fs::path sourceFilePath{directoryEntry.path()};
		const std::size_t sourceLineNumber{getFileLineNumber(sourceFilePath)};

		//Check that all lang files are complete
		for(auto& availableLang : langs.getAvailableLangs())
		{
			const fs::path translationFilePath{resourcesPath / availableLang / sourceFilePath.filename()};
			BOOST_CHECK_MESSAGE(sourceLineNumber == getFileLineNumber(translationFilePath),
					"It seems that lang files are not consistent (" + sourceFilePath.string() + " and " + translationFilePath.string() + ").");
		}
	}
}

BOOST_AUTO_TEST_CASE(setLangTests)
{
	const std::string defaultLang{langs.getDefaultLang()}, testedLang{"FR"};
	langs.setLang(testedLang);
	BOOST_CHECK(langs.getLang() == testedLang);
	const std::string notExistingLang{"Not existing lang"};
	BOOST_CHECK_THROW(langs.setLang(notExistingLang), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
