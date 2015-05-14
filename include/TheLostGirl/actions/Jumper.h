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
	/// Default constructor.
	Jumper();

	/// Default destructor
	virtual ~Jumper();

	/// Overload of the () operator.
	/// \param entity Entity who jump.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
};

#endif//JUMPER_H
