#include <iostream>
#include <fstream>
#include <exception>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/LangManager.hpp>

namespace fs = boost::filesystem;

LangManager::LangManager():
	m_defaultLang{"EN"},
	m_langsDirectoryPath{Context::getParameters().resourcesPath / fs::path{"lang"}}
{
}

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

std::set<std::string> LangManager::getAvailableLangs() const
{
	std::set<std::string> res;
	for(auto& directoryEntry : fs::directory_iterator(m_langsDirectoryPath))
		if(fs::is_directory(directoryEntry.status()))
			res.insert(directoryEntry.path().filename().generic_string());
	return res;
}

std::wstring LangManager::tr(const std::string& entryName) const
{
	auto it(m_entries.find(entryName));
	if(it != m_entries.end())
		return it->second;
	else
	{
		std::cerr << "No translation available for \"" << entryName << "\" in the lang " << m_lang << std::endl;
		return std::wstring(entryName.begin(), entryName.end());
	}
}

void LangManager::loadLang(const std::string& langToLoad)
{
	//Get the path of both source and translation directories
	const fs::path sourceDirectoryPath{m_langsDirectoryPath / m_defaultLang};
	const fs::path translationDirectoryPath{m_langsDirectoryPath / langToLoad};
	assert(fs::is_directory(sourceDirectoryPath));
	assert(fs::is_directory(translationDirectoryPath));
	m_entries.clear();//First, clear all known translations
	
	for(auto& directoryEntry : fs::directory_iterator(sourceDirectoryPath))
	{
		//Get the path of both source and translation files
		const fs::path sourceFilePath{directoryEntry.path()};
		const fs::path translationFilePath{translationDirectoryPath / sourceFilePath.filename()};
		
		//Check that the source file and the translation file are regular files
		if(not fs::is_regular_file(sourceFilePath) or not fs::is_regular_file(translationFilePath))
			continue;
		
		fs::ifstream sourceFileStream{sourceFilePath};
		fs::wifstream translationFileStream;

		//Things to handle wide encoding
		translationFileStream.imbue(std::locale(""));
		translationFileStream.open(translationFilePath);

		//Check if the langs files are loaded
		if(not sourceFileStream.is_open())
			throw std::runtime_error("Unable to open source lang file: " + sourceFilePath.string());
		else if(not translationFileStream.is_open())
			throw std::runtime_error("Unable to open translation lang file: " + translationFilePath.string());

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
	}
}
