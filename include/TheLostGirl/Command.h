#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
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
	/// Constructor.
	Command();

	/// Copy constructor.
	/// It must be implemented in order to assign the right variable
	/// (category or entity) if targetIsSpecific is true or not.
	/// \param other Command to copy.
	Command(const Command& other);

    /// Stored function.
    /// \param entityx::Entity Entity which do the action.
    /// That function can be a lambda expression, a functor, a pointer to a function, a member function, ...
    /// It just need to take this argument as parameter.
	std::function<void(entityx::Entity)> action;

	/// Indicates if the command must be applied on a specific entity
	/// or on a global group en entities. If true, the action must be
	/// applied on the entity pointer, and if false on every entity with the category.
	bool targetIsSpecific;

	union
	{
		/// Category of entity concerned by \a action.
		unsigned int category;

		/// Specific entity concerned by \a action.
		entityx::Entity entity;
	};
};

#endif//COMMAND_H
