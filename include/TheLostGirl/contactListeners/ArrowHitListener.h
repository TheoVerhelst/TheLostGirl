#ifndef ARROWHITLISTENER_H
#define ARROWHITLISTENER_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include <TheLostGirl/State.h>

/// Collision listener.
/// - Handle collisions of arrows to decide if the arrow should be sticked
/// on the other fixture.
class ArrowHitListener : public b2ContactListener
{
	public:
		///Default constructor.
        /// \param context Current context of the application.
		ArrowHitListener(State::Context context);
		
		/// This is called after a contact is updated.
		/// This allows to inspect a contact before it goes to the solver.
		/// It prevent the collision between entities with the same entity Id.
		/// \param contact Structure containing data about the contact.
		/// \param oldManifold Old manifold.
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
		/// \param contact Structure containing data about the contact.
		/// \param impulse Resulting impulse.
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	
	private:
		State::Context m_context;///< Current context of the application.
};

#endif // ARROWHITLISTENER_H
