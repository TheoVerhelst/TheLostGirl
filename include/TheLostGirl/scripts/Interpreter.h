#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <map>
#include <fstream>

#include <boost/variant.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/scripts/Tree.h>
#include <TheLostGirl/StateStack.h>

typedef boost::variant<bool, int, float, std::string, entityx::Entity> Data;
typedef std::list<std::string> VecStr;

class Interpreter
{
	public:
		Interpreter();
		~Interpreter();
		bool loadFromFile(const std::string& fileName);
		void interpret(entityx::Entity entity, StateStack::Context context);

	private:
		struct Function
		{
			short int numberOperands;
			std::function<Data(const std::vector<Data>&, StateStack::Context)> pointer;
		};
		std::ifstream m_file;///< Stream of the script file to execute.
		const std::map<std::string, int> m_precedence;///< Precedence of all binary and unary operators.
		const std::map<std::string, Function> m_functions;///< All functions available in the script.
		const std::map<std::string, Data> m_initialVars;///< Variables available by default in all scripts.
		std::map<std::string, Data> m_vars;///< Variables of the current script.
		StateStack::Context* m_context;    ///< The current context of the application.

		std::string m_operators;
		const std::vector<std::vector<std::string>> m_multichar0perators;
		const std::vector<std::string> m_reservedNames;

		void interpretBlock(VecStr::iterator from, VecStr::iterator to, VecStr::iterator begin);
		Tree<Data>::Ptr convert(const VecStr& tokens);
		VecStr tokenize(const std::string& line) const;
		Data evaluateToken(const std::string& token) const;
		Data evaluateTree(const Tree<Data>::Ptr expression) const;

		size_t parenthesis(const VecStr& tokens) const;

		/// Check if the given string contains only whitespaces.
		/// \param str String to check.
		/// \return True if the given string contains only whitespaces, false otherwise.
		inline bool isSpace(const std::string& str) const;

		/// Check if the given string represents a number, floating or integer.
		/// \param str String to check.
		/// \return True if the given string represents a number, false otherwise.
		inline bool isNumber(const std::string& str) const;

		/// Check if the given string represents an identifier.
		/// \param str String to check.
		/// \return True if the given represents an identifier, false otherwise.
		inline bool isIdentifier(const std::string& str) const;

		/// Check if the given string represents an boolean.
		/// \param str String to check.
		/// \return True if the given string represents an boolean, false otherwise.
		inline bool isBool(const std::string& str) const;

		/// Check if the given string represents an string literal.
		/// \param str String to check.
		/// \return True if the given string represents an string literal, false otherwise.
		inline bool isString(const std::string& str) const;

		inline bool isValue(const std::string& str) const;

		/// Return a string without trailing blank.
		/// \param str The string to strip.
		/// \return The stripped string.
		std::string strip(const std::string& str) const;
};

template <typename T>
void cast(Data& var)
{
}

template <>
void cast<bool>(Data& var);

template <>
void cast<int>(Data& var);

template <>
void cast<float>(Data& var);

template <>
void cast<std::string>(Data& var);

template <>
void cast<entityx::Entity>(Data& var);

#endif// INTERPRETER_HPP
