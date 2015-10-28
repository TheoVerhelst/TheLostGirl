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
	/// Destructor.
	virtual ~Action() = default;

	/// Overload of the () operator.
	/// \param entity Entity to do the action on.
	/// \param dt Elapsed time in the last game frame.
	//TODO reflechir a la surcharge
	virtual void operator()(entityx::Entity entity, double dt) const = 0;
};

#endif//ACTION_H
