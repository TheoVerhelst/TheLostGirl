#ifndef ARROWPICKER_H
#define ARROWPICKER_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Structure that pick a sticked arrow nearby the entity.
struct ArrowPicker : public Action
{
	/// Constructor.
	ArrowPicker();

	/// Destructor.
	virtual ~ArrowPicker();

	/// Overload of the () operator.
	/// \param entity Entity who shoot.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;
};

/// AABB query callback that indicate if a sticked arrow is found into the AABB.
class StickedArrowQueryCallback : public b2QueryCallback
{
	public:
		/// Handle the world querying.
		/// \param fixture A fixture that overlap the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		entityx::Entity foundEntity;///< If no arrow is found, then this entity won't be valid.
};

#endif//ARROWPICKER_H

