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
	/// \param entity Entity which do the action.
	virtual void operator()(entityx::Entity entity) const = 0;
};

#endif//ACTION_H
