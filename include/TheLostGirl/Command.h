#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <queue>

//Forward declarations
namespace entityx
{
	class Entity;
}
enum class Category;

///Function applied to an entity.
///The Command structure hold a function that will be applied to a specific entity.
struct Command
{
	Command();
	
    ///Stored function.
    /// \param entityx::Entity& Entity to wich apply the function.
    /// \param double Elapsed time between the current and the last frame.
    ///That function can be a lambda expression, a functor, a pointer to a function, a member function, ...
    ///It just need to take these two arguments in parameters.
	std::function<void(entityx::Entity&, double dt)> action;
	
    ///Category
    ///Category of entity concerned by \a action. The user must himself check category before call \a action.                
	Category category;
};

///Convenient typedef of Command.
typedef std::queue<Command> CommandQueue;

#endif // COMMAND_H
