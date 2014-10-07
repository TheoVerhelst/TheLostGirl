#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include <string>
#include <locale>
#include <fstream>
#include <map>
#include <exception>

///Available langages.
enum Lang
{
	EN,///< The English
	FR,///< The French
	NL,///< The Dutch
	IT///< The Italian
};

///Language manager that translate strings.
///It translate a std::string in english to a std::wtring in the langage defined by setLang.
///If no langage is set, it will return L"Entry not found." at every call of LangManager::tr.
class LangManager
{
	public:
        /// Set the current lang.
        /// This function is static, so it should be called by only one user.
        /// The lang file is automaticaly loaded by the manager.
        /// \param newLang Lang to load
        /// \return void
		static void setLang(Lang newLang);

        /// Return the current lang.
        /// \return Lang
		static Lang getLang();

        /// Translate the given string.
        ///	The \a entryName must be an entry in the lang file, and be in english.
        /// Write LangManager::tr() around every in-game text.
        /// \param entryName const std::string&
        /// \return The translated wide string if the string can be translated, or a default error string in the current lang otherwise.
		static std::wstring tr(const std::string& entryName);

	private:
		static void loadLang(Lang langToLoad);
		static Lang m_lang;
		static std::map<std::string, std::wstring> m_entries;
};

#endif // LANGMANAGER_H
