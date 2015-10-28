#ifndef JUMPER_H
#define JUMPER_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Structure that jump an entity.
struct Jumper : public Action
{
	/// Constructor.
	Jumper() = default;

	/// Destructor.
	virtual ~Jumper() = default;

	/// Overload of the () operator.
	/// \param entity Entity who jump.
	virtual void operator()(entityx::Entity entity) const;
};

#endif//JUMPER_H

