#ifndef ACTION_H
#define ACTION_H

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Base class for every action.
struct Action
{
	/// Default destructor
	virtual ~Action();
	
	/// Overload of the () operator.
	/// \param entity Entity to do the action on.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const = 0;
};

#endif // ACTION_H
