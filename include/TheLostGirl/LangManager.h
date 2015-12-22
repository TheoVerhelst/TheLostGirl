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
	EN,///< English.
	FR ///< French.
};

/// Language manager that translates strings.
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

		/// Get the default lang.
		/// The default lang is the one that is used in code,
		/// and that is loaded if the file for the asked lang is not found.
		/// \return The default lang.
		Lang getDefaultLang() const;

		/// Get a string representing the given lang,
		/// this string is intented to be a valid lang file
		/// in the directory "lang".
		/// \param lang The lang to cast to a string.
		/// \return The string representation of the parameter.
		static std::string getLangName(Lang lang);

		/// Get an array of all available langs.
		/// \return An array of all available langs.
		static std::array<Lang, 2> getAvailableLangs();

        /// Translate the given string.
        ///	The \a entryName must be an entry in the lang file, and be in english.
        /// Should be called for every in-game text.
        /// \param entryName const std::string&
        /// \return The translated wide string if the string can be translated, or entryName as std::wstring otherwise.
		std::wstring tr(const std::string& entryName) const;

	private:
		/// Load the data related to the given lang.
		/// \param langToLoad Lang to load.
		void loadLang(Lang langToLoad);

		Lang m_lang;                                  ///< The current lang.
		const Lang m_defaultLang = EN;
		std::map<std::string, std::wstring> m_entries;///< Translations related to the current lang.
		static const std::map<Lang, std::string> m_langNames;
};

#endif//LANGMANAGER_H
