#include <iostream>
#include <TheLostGirl/LangManager.h>

#define MAP_ITEM(lang) {lang, #lang}
const std::map<Lang, std::string> LangManager::langNames = {MAP_ITEM(EN), MAP_ITEM(FR), MAP_ITEM(NL), MAP_ITEM(IT)};
#undef MAP_ITEM

void LangManager::setLang(Lang newLang)
{
	m_lang = newLang;
	loadLang(m_lang);
}

std::wstring LangManager::tr(const std::string& entryName) const
{
	if(m_entries.count(entryName) > 0)
		return m_entries.at(entryName);
	else
	{
		std::cerr << "No translation available for \"" << entryName << "\" in the lang " << langNames.at(m_lang) << std::endl;
		return std::wstring(entryName.begin(), entryName.end());
	}
}

Lang LangManager::getLang() const
{
	return m_lang;
}

void LangManager::loadLang(Lang langToLoad)
{
	m_entries.clear();
	std::string sourceFilePath("resources/lang/EN"), translationFilePath(sourceFilePath);
	std::ifstream sourceFileStream;
	std::wifstream translationFileStream;
	//Things to handle wide encoding
	std::locale loc("");
	translationFileStream.imbue(loc);
	switch(langToLoad)//Open the right file
	{
		case NL:
		case IT:
		case EN:
			translationFilePath = "resources/lang/EN";
			break;

		case FR:
			translationFilePath = "resources/lang/FR";
			break;
	}
	sourceFileStream.open(sourceFilePath);
	translationFileStream.open(translationFilePath);
	if(not sourceFileStream.is_open())
		throw std::runtime_error("Unable to open source lang file: " + sourceFilePath);
	else if(not translationFileStream.is_open())
		throw std::runtime_error("Unable to open translation lang file: " + translationFilePath);
	else
	{
		while(not sourceFileStream.eof() and not translationFileStream.eof())
		{
			std::string sourceLine;
			std::wstring translatedLine;
			getline(sourceFileStream, sourceLine);//fill line with the next line of filestream
			getline(translationFileStream, translatedLine);//fill line with the next line of filestream
			m_entries.emplace(sourceLine, translatedLine);
		}
	}
	if(sourceFileStream.eof() != translationFileStream.eof())
	{
		std::cerr << "\"" << sourceFilePath << "\" source file and \"" << translationFilePath << "\" translation file have different number of lines." << std::endl;
		std::cerr << "Translations are probably inconsistent." << std::endl;
	}
	sourceFileStream.close();
	translationFileStream.close();
}
