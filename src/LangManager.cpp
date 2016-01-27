#include <iostream>
#include <fstream>
#include <exception>
#include <boost/filesystem.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/LangManager.hpp>

void LangManager::setLang(const std::string& newLang)
{
	if(newLang != m_lang)
	{
		m_lang = newLang;
		loadLang(m_lang);
	}
}

std::string LangManager::getLang() const
{
	return m_lang;
}

std::string LangManager::getDefaultLang() const
{
	return m_defaultLang;
}

std::set<std::string> LangManager::getAvailableLangs()
{
	std::set<std::string> res;
	for(auto& directory_entry : boost::filesystem::directory_iterator(Context::parameters->resourcesPath + "lang/"))
		res.insert(directory_entry.path().filename().generic_string());
	return res;
}

std::wstring LangManager::tr(const std::string& entryName) const
{
	if(m_entries.count(entryName) > 0)
		return m_entries.at(entryName);
	else
	{
		std::cerr << "No translation available for \"" << entryName << "\" in the lang " << m_lang << std::endl;
		return std::wstring(entryName.begin(), entryName.end());
	}
}

void LangManager::loadLang(const std::string& langToLoad)
{
	std::string sourceFilePath{Context::parameters->resourcesPath + "lang/" + m_defaultLang};
	std::string translationFilePath{Context::parameters->resourcesPath + "lang/" + langToLoad};
	std::ifstream sourceFileStream;
	std::wifstream translationFileStream;
	m_entries.clear();

	//Things to handle wide encoding
	translationFileStream.imbue(std::locale(""));

	sourceFileStream.open(sourceFilePath);
	translationFileStream.open(translationFilePath);

	//Check if the langs files are loaded
	if(not sourceFileStream.is_open())
		throw std::runtime_error("Unable to open source lang file: " + sourceFilePath);
	else if(not translationFileStream.is_open())
	{
		std::cerr << "Unable to open translation lang file: " + translationFilePath << std::endl;
		//Load default translation file
		translationFileStream.open(sourceFilePath);
	}
	if(not sourceFileStream.is_open())
		throw std::runtime_error("Unable to open default translation lang file: " + sourceFilePath);

	std::string sourceLine;
	std::wstring translatedLine;
	while(not sourceFileStream.eof() and not translationFileStream.eof())
	{
		getline(sourceFileStream, sourceLine);//fill line with the next line of filestream
		getline(translationFileStream, translatedLine);//fill line with the next line of filestream
		m_entries.emplace(sourceLine, translatedLine);
	}

	if(sourceFileStream.eof() != translationFileStream.eof())
	{
		std::cerr << "\"" << sourceFilePath << "\" source file and \"" << translationFilePath << "\" translation file have different number of lines." << std::endl;
		std::cerr << "Translations are probably inconsistent." << std::endl;
	}

	sourceFileStream.close();
	translationFileStream.close();
}
