#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <map>
#include <fstream>
#include <TheLostGirl/scripts/Variant.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/scripts/Tree.hpp>
#include <TheLostGirl/components.hpp>

typedef Variant<bool, int, float, std::string, entityx::Entity, Direction> Data;

namespace DataType
{
	enum : std::size_t
	{
		Boolean,
		Integer,
		Float,
		String,
		Entity,
		Direction
	};
}

typedef Tree<std::pair<Data, bool>> Expression;

/// Loads, evaluates and applies a script on an entity.
/// This class has an interface similar to SFML resources, since it implements
/// an loadFromFile function. So Interpreter instances can be handled by a
/// ResourceManager.
/// Internally, this class opens the asked file with loadFromFile(), and convert
/// every line of this one to a tree that will be evaluated in the game loop,
/// with the function interpret().
class Interpreter
{
	public:
		/// Constructor.
		Interpreter();

		/// Loads the given script file in order to execute it later.
		/// This function parses the file and may throw exceptions if the file
		/// is badly written. If not so, it stores the whole script in a list of
		/// trees internally and return true.
		/// The function must return a boolean in order to be compatible with
		/// other SFML ressource loading functions.
		/// \param fileName The name of the file to load.
		/// \return True on succes.
		bool loadFromFile(const std::string& fileName);

		/// Executes the loaded script on the entity. The entity parameter is
		/// the entity that is available as the variable "self" in the scripts.
		/// \param entity The entity on wich execute the script.
		void interpret(entityx::Entity entity);

	private:
		/// Represents a function that can be called in scripts.
		struct Function
		{
			short int numberOperands;///< Number of operands that the function must take. If -1, the function is variadic.
			std::function<Data(const std::vector<Data>&)> pointer;///< Pointer to the actual C++ function to execute.
		};

		std::ifstream m_file;                                        ///< Stream of the script file to execute.
		const std::map<std::string, int> m_precedence;               ///< Precedence of all binary and unary operators.
		const std::map<std::string, Function> m_functions;           ///< All functions available in the script.
		const std::map<std::string, Data> m_initialVars;             ///< Variables available by default in all scripts.
		std::map<std::string, Data> m_vars;                          ///< Variables of the current script.

		std::list<Expression::Ptr> m_expressions;                    ///< List of all expression of the script, stored as trees.
		std::string m_operators;                                     ///< List of characters that will separates tokens in tokenization process.
		const std::list<std::list<std::string>> m_multichar0perators;///< List of all operators that are made from more than one characters.
		const std::list<std::string> m_reservedNames;                ///< List of all reserved names that cannot be a part of a variable identifier.

		/// Evaluates a block of instructions.
		/// The iterators should be iterator on m_tree.
		/// \param entity The entity on which executing the script.
		/// \param from Iterator to the first expression to evaluate, in a
		/// std::list.
		/// \param to Iterator to the past-the-last expression to evaluate, in a
		/// std::list.
		/// \param begin Iterator to the very first expression of the script, it
		/// is only used in error messages to display the correct line number.
		void interpretBlock(entityx::Entity entity, std::list<Expression::Ptr>::iterator from,
				std::list<Expression::Ptr>::iterator to, std::list<Expression::Ptr>::iterator begin);

		/// Shortening for a tedious operation, gets the string value of the
		/// tree pointer pointed by a list iterator.
		/// \param it Iterator that points to the tree pointer that holds the
		/// desired string value.
		/// \return The desired string value.
		inline std::string getStr(std::list<Expression::Ptr>::iterator it) const;

		/// Converts a list of tokens to a tree, and take care of operator
		/// precedence.
		/// \param from Iterator to the beginning of the line to convert.
		/// \param to Iterator to the past-the-end of the line to convert.
		/// \return A pointer to the resulting tree.
		Expression::Ptr convertTokens(std::list<std::string>::const_iterator from,
				std::list<std::string>::const_iterator to) const;

		/// Splits in tokens the given line.
		/// All algorithms in this function are hardcoded bevause STL and boost
		/// alternative are sometimes slow.
		/// \param from Iterator to the beginning of the line to tokenize.
		/// \param to Iterator to the past-the-end of the line to tokenize.
		/// \return A list of tokens.
		std::list<std::string> tokenize(std::string::const_iterator from, std::string::const_iterator to) const;

		/// Recursively evaluates the given tree that represents an expression,
		/// and returns the final value.
		/// \param expression Tree to evaluate.
		/// \return The resulting data.
		Data evaluateExpression(const Expression::Ptr expression);

		/// Returns the value of the given token that represents a string
		/// literal.
		/// \param token Token to evaluate.
		/// \return The raw data represented by the token.
		Data evaluateStringToken(const std::string& token) const;

		/// Finds the first closing parenthesis.
		/// This function takes care of passing over nested parentheses.
		/// \param from Iterator to the first token after the opening
		/// parenthesis.
		/// \param to Iterator to the past-the-last token that can be the
		/// corresponding parenthesis.
		/// \return An iterator to the parenthesis if found, \a to otherwise.
		std::list<std::string>::const_iterator parenthesis(std::list<std::string>::const_iterator from,
				std::list<std::string>::const_iterator to) const;

		/// Checks if the given string contains only whitespaces.
		/// \param str String to check.
		/// \return True if the given string contains only whitespaces, false
		/// otherwise.
		inline bool isSpace(const std::string& str) const;

		/// Checsks if the given string represents a floating point number.
		/// \param str String to check.
		/// \return True if the given string represents a floating point number,
		/// false otherwise.
		inline bool isFloat(const std::string& str) const;

		/// Checks if the given string represents an integer number.
		/// \param str String to check.
		/// \return True if the given string represents an integer number, false
		/// otherwise.
		inline bool isInt(const std::string& str) const;

		/// Checks if the given string represents an identifier.
		/// \param str String to check.
		/// \return True if the given represents an identifier, false otherwise.
		inline bool isIdentifier(const std::string& str) const;

		/// Check if the given string represents an boolean.
		/// \param str String to check.
		/// \return True if the given string represents an boolean, false
		/// otherwise.
		inline bool isBool(const std::string& str) const;

		/// Checks if the given string represents an string literal.
		/// \param str String to check.
		/// \return True if the given string represents an string literal, false
		/// otherwise.
		inline bool isString(const std::string& str) const;

		/// Returns a string without trailing blank.
		/// \param str The string to strip.
		/// \return The stripped string.
		std::string strip(const std::string& str) const;
};

#endif//INTERPRETER_HPP
