#ifndef HANDTOHAND_H
#define HANDTOHAND_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

struct HandToHand : public Action
{
	/// Default constructor.
	HandToHand();

	/// Default destructor
	virtual ~HandToHand();

	/// Overload of the () operator.
	/// \param entity Entity who do the action.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;

	private:
};

/// AABB query callback that indicate if an actor is found into the AABB.
class HandToHandQueryCallback : public b2QueryCallback
{
	public:
		/// Constructor.
		/// \param attacker The entity that does the attack.
		HandToHandQueryCallback(entityx::Entity attacker);

		/// Handle the world querying.
		/// \param fixture A fixture that overlap the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		entityx::Entity foundEntity;///< If no actor is found, then this entity won't be valid.

	private:
		entityx::Entity m_attacker;
};

#endif//HANDTOHAND_H

