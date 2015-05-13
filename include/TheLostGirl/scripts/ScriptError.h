#include <stdexcept>


/// Exception related to scripts parsing or interpretation.
class ScriptError: public std::runtime_error
{
	public:
		/// Constructor
		/// \param what_arg Message describing the error.
		ScriptError(const std::string what_arg="") throw():
			std::runtime_error(what_arg),
			m_what(what_arg)
		{}

		/// Get the error message.
		/// \return The error message.
		virtual const char* what() const throw()
		{
			return m_what.c_str();
		}

		/// Destructor.
		virtual ~ScriptError() throw()
		{}

	private:
		const std::string m_what;///< The error message.
};
