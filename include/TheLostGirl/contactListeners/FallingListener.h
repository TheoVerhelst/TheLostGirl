#ifndef FALLINGLISTENER_H
#define FALLINGLISTENER_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

/// Collision listener.
/// - Handle collisions between falling actors and others entities 
/// to set the right animation and update the FallComponent.
class FallingListener : public b2ContactListener
{
	public:
		/// This is called after a contact is updated.
		/// This allows to inspect a contact before it goes to the solver.
		/// It prevent the collision between entities with the same actor ID.
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		
		/// Handle the hitting of an actor with the ground.
		/// Function called on every begin of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void BeginContact(b2Contact* contact);
		
		/// Handle the end of contact between the ground and an actor.
		/// Function called on every end of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void EndContact(b2Contact* contact);
		
		/// This is called after the solver is finished.
		/// This allows to inspect e.g. impulses.
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

#endif // FALLINGLISTENER_H