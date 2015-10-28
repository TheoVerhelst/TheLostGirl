#include <TheLostGirl/LangManager.h>

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
		return std::wstring(entryName.begin(), entryName.end());
}

Lang LangManager::getLang() const
{
	return m_lang;
}

void LangManager::loadLang(Lang langToLoad)
{
	m_entries.clear();
	std::string filePath;
	std::wifstream fileStream;
	//Things to handle wide encoding
	std::locale loc("");
	fileStream.imbue(loc);
	switch(langToLoad)//Open the right file
	{
		case EN:
			filePath = "resources/lang/EN";
			break;

		case FR:
			filePath = "resources/lang/FR";
			break;

		case NL:
		case IT:
		default:
			filePath = "resources/lang/EN";
			break;
	}
	fileStream.open(filePath);
	if(not fileStream.is_open())//If failed to open the file
		throw std::runtime_error("Unable to open lang file: " + filePath);//Throw an exception
	else
	{
		while(not fileStream.eof())
		{
			std::wstring line;
			getline(fileStream, line);//fill line with the next line of filestream
			std::size_t barPos{line.find(L"|")};
			std::string entryNameString;
			std::wstring entryNameWString{line.substr(0, barPos)};//All before the |
			entryNameString.assign(entryNameWString.begin(), entryNameWString.end());//Convert wstring to string
			std::wstring value{line.substr(barPos + 1)}; //All after the |
			m_entries.emplace(entryNameString, value);
		}
	}
	fileStream.close();
}
