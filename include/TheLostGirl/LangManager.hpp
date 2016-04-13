#ifndef LANGMANAGER_HPP
#define LANGMANAGER_HPP

#include <string>
#include <map>
#include <set>
#include <TheLostGirl/Context.hpp>

/// Language manager that translates strings.
/// It translate a std::string in english to a std::wtring in the langage defined by setLang.
/// If an error occurs, such as when no language is set or when no translation in found, the original string is returned.
class LangManager
{
	public:
        /// Set the current lang.
        /// The lang file is automaticaly loaded by the manager.
        /// \param newLang The lang to load.
        /// \throw std::runtime_error if the lang is not available.
		void setLang(const std::string& newLang);

        /// Return the current lang.
        /// \return The current lang.
		std::string getLang() const;

		/// Get the default lang.
		/// The default lang is the one that is used in code,
		/// and that is loaded if the file for the asked lang is not found.
		/// \return The default lang.
		std::string getDefaultLang() const;

		/// Get an array of all available langs.
		/// \return An array of all available langs.
		static std::set<std::string> getAvailableLangs();

        /// Translate the given string.
        ///	The \a entryName must be an entry in the lang file, and be in english.
        /// Should be called for every in-game text.
        /// \param entryName const std::string&
        /// \return The translated wide string if the string can be translated, or entryName as std::wstring otherwise.
		std::wstring tr(const std::string& entryName) const;

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::Parameters> Context;

		/// Load the data related to the given lang.
		/// \param langToLoad Lang to load.
		void loadLang(const std::string& langToLoad);

		std::string m_lang;                           ///< The current lang.
		const std::string m_defaultLang = "EN";       ///< The default lang used in the source code.
		std::map<std::string, std::wstring> m_entries;///< Translations related to the current lang.
};

#endif//LANGMANAGER_HPP
