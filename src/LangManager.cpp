#include <TheLostGirl/LangManager.h>

using namespace std;

Lang LangManager::m_lang;
map<string, wstring> LangManager::m_entries;

void LangManager::setLang(Lang newLang)
{
	m_lang = newLang;
	loadLang(m_lang);
}

wstring LangManager::tr(const string& entryName)
{
	if(m_entries.count(entryName) > 0)
		return m_entries[entryName];
	else
	{
		//Return an error message
		switch(m_lang)
		{
			case EN:
				return L"Entry not found.";

			case FR:
				return L"Entrée non trouvée.";

			default:
				return L"Entry not found.";
		}
	}
}

Lang LangManager::getLang()
{
	return m_lang;
}

void LangManager::loadLang(Lang langToLoad)
{
	string filePath;
	wifstream fileStream;
	setlocale(LC_ALL, "");//Things to open the file with a wide character table
	locale loc("");
	fileStream.imbue(loc);
	switch(langToLoad)//Open the right file
	{
		case EN:
			filePath = "ressources/lang/EN";
			break;

		case FR:
			filePath = "ressources/lang/FR";
			break;

		default:
			filePath = "ressources/lang/EN";
			break;
	}
	fileStream.open(filePath);
	if(not fileStream.is_open())//If failed to open the file
		throw runtime_error("Unable to open lang file : " + filePath);//Throw an exception
	else
	{
		while(not fileStream.eof())
		{
			wstring line;
			getline(fileStream, line);//fill line with the next line of filestream
			size_t barPos = line.find(L"|");
			string entryNameString;
			wstring entryNameWString = line.substr(0, barPos);//All before the |
			entryNameString.assign(entryNameWString.begin(), entryNameWString.end());//Convert wstring to string
			wstring value = line.substr(barPos + 1); //All after the |
			m_entries.emplace(entryNameString, value);
		}
		fileStream.close();
	}
}
