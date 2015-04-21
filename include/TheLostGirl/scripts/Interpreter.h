#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <map>
#include <regex>
#include <fstream>

#include <boost/variant.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/scripts/Tree.h>
#include <TheLostGirl/StateStack.h>

typedef boost::variant<bool, int, float, std::string, entityx::Entity> Data;
typedef std::vector<std::string> VecStr;

class Interpreter
{
	public:
		Interpreter(std::ifstream& file, entityx::Entity entity, StateStack::Context context);
		void interpret();

	private:
		struct Function
		{
			short int numberOperands;
			std::function<Data(const std::vector<Data>&, StateStack::Context)> pointer;
		};
		std::ifstream& m_file;
		const std::map<std::string, int> m_precedence;
		const std::map<std::string, Function> m_functions;
		std::map<std::string, Data> m_vars;
		StateStack::Context m_context;    ///< The current context of the application.

		const std::string identifier;
		const std::string number_literal;
		const std::string boolean_literal;
		const std::string string_literal;
		const std::string operators;
		const std::string other;
		const std::string reserved_names;

		const std::regex identifier_regex;
		const std::regex number_literal_regex;
		const std::regex boolean_literal_regex;
		const std::regex string_literal_regex;
		const std::regex reserved_names_regex;
		const std::regex token_regex;
		const std::regex value_regex;
		const std::regex space_regex;

		void interpretBlock(VecStr::iterator from, VecStr::iterator to, VecStr::iterator begin);
		Tree<Data>::Ptr convert(const VecStr& tokens);
		std::vector<std::string> tokenize(std::string line) const;
		Data evaluateToken(const std::string& token) const;
		Data evaluateTree(const Tree<Data>::Ptr expression) const;
		size_t parenthesis(const VecStr& tokens) const;
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
