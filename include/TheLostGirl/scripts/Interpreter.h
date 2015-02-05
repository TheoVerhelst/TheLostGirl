#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <queue>

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <entityx/Entity.h>

//Forward declarations
struct Command;

/// Interpreter of The Lost Girl scripts.
/// Construct it with the filestream to execute,
/// and run interpret().
class Interpreter
{
	public:
	    /// Constructor.
	    /// \param file Stream to the script to execute.
		Interpreter(std::ifstream& file, entityx::Entity entity, std::queue<Command>& commandQueue);

		/// Execute the given script.
		void interpret();

	private:
	    /// Variable in scripts, can have up to 4 differents types.
		typedef boost::variant<int, float, bool, entityx::Entity> Var;

        /// Divides in tokens the given line.
        /// \param line The line to tokenize.
        /// \return An array of tokens.
		std::vector<std::string> tokenize(std::string line);

		/// Calculates the value.
		/// \param tokens The array of tokens to compute.
		/// \return A variant value.
		Var compute(std::vector<std::string> tokens);

        /// Seek the script to the next else or the next endif.
        /// \param jump_to_else True if the function must seek to the next else,
        /// false if the function must seek to the next endif.
		void jump_to_endif(bool jump_to_else = false);

		/// Convert the variable to a concrete type.
		/// \param var Variable to convert.
		/// \return The converted value.
		template <typename T>
		T convert(Var var);

		std::ifstream& m_file;              ///< Stream to the script.
		std::map<std::string, Var> m_vars;  ///< Set of registered variables.
		entityx::Entity m_entity;		    ///< Entity that run the script.
		std::queue<Command>& m_commandQueue;///< List of all commands that need to be executed.

};

template <typename T>
T Interpreter::convert(Var var)
{
    if(var.which() == 3)
    {
        if(typeid(T) != typeid(bool))
            throw std::runtime_error("entity type cannot be converted to anything else than boolean.");
        else
            return (T)boost::get<entityx::Entity>(var);
    }
	else if(var.which() == 2)
		return (T)boost::get<bool>(var);
	else if(var.which() == 1)
		return (T)boost::get<float>(var);
	else
		return (T)boost::get<int>(var);
}
