#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include <string>
#include <locale>
#include <fstream>
#include <map>
#include <exception>

/// Available langages.
enum Lang
{
	EN,///< The English
	FR,///< The French
	NL,///< The Dutch
	IT///< The Italian
};

/// Language manager that translate strings.
/// It translate a std::string in english to a std::wtring in the langage defined by setLang.
/// If an error occurs, such as when no language is set or when no translation in found, the original string is returned.
class LangManager
{
	public:
        /// Set the current lang.
        /// The lang file is automaticaly loaded by the manager.
        /// \param newLang Lang to load
        /// \return void
		void setLang(Lang newLang);

        /// Return the current lang.
        /// \return Lang
		Lang getLang() const;

        /// Translate the given string.
        ///	The \a entryName must be an entry in the lang file, and be in english.
        /// Should be called for every in-game text.
        /// \param entryName const std::string&
        /// \return The translated wide string if the string can be translated, or entryName as std::wstring otherwise.
		std::wstring tr(const std::string& entryName) const;

	private:
		/// Load the data related to the given lang.
		/// \param langToLoad Lang to load.
		static void loadLang(Lang langToLoad);

		static Lang m_lang;                                  ///< The current lang.
		static std::map<std::string, std::wstring> m_entries;///< Translations related to the current lang.
};

#endif//LANGMANAGER_H
