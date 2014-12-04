#ifndef CONTACTFILTER_H
#define CONTACTFILTER_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include <TheLostGirl/State.h>

/// Collision filter.
/// - Disable collision between multiples body of the same actor.
/// - Disable collision between bodies of differents plans.
class ContactFilter : public b2ContactFilter
{
	public:
		/// Indicate if the two fixtures should collide or not.
		/// \param fixtureA First fixture.
		/// \param fixtureB Second fixture.
		/// \return True if the two fixtures should collide, false otherwise.
		virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

#endif//CONTACTFILTER_H
