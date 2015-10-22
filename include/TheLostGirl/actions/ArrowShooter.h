#ifndef ARROWSHOOTER_H
#define ARROWSHOOTER_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Structure that shoot the arrow notched by the entity.
struct ArrowShooter : public Action
{
	/// Constructor.
	ArrowShooter() = default;

	/// Destructor.
	virtual ~ArrowShooter() = default;

	/// Overload of the () operator.
	/// \param entity Entity who shoot.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
};

#endif//ARROWSHOOTER_H

