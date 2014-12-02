#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <queue>

#include <TheLostGirl/Category.h>

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Function applied to an entity.
/// The Command structure hold a function that will be applied to a specific entity.
struct Command
{
	Command();
	
    /// Stored function.
    /// \param entityx::Entity Entity to wich to apply the function.
    /// \param double Elapsed time between the current and the last frame.
    /// That function can be a lambda expression, a functor, a pointer to a function, a member function, ...
    /// It just need to take these two arguments in parameters.
	std::function<void(entityx::Entity, double)> action;
	
	/// Indicates if the command must be applied on a specific entity
	/// or on a global group en entities. If true, the action must be
	/// applied on the entity pointer, and if false on every entity with the category.
	bool targetIsSpecific = false;
	
	union
	{
		/// Category of entity concerned by \a action.
		unsigned int category = 0;
		
		/// Specific entity concerned by \a action.
		entityx::Entity* entity;
	};
};

/// Convenient typedef of Command.
typedef std::queue<Command> CommandQueue;

#endif // COMMAND_H
