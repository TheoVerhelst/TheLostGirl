#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/contactListeners/FallingListener.h>
#include <TheLostGirl/contactListeners/ArrowHitListener.h>
#include <TheLostGirl/StateStack.h>

/// Collision listener.
/// - Handle collisions between falling actors and others entities
/// to set the right animation and update the FallComponent.
class ContactListener : public b2ContactListener
{
	public:
		/// This is called after a contact is updated.
		/// This allows to inspect a contact before it goes to the solver.
		/// It prevent the collision between entities with the same entity Id.
		/// \param contact Structure containing data about the contact.
		/// \param oldManifold Old manifold.
		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

		/// Handle the hitting of an actor with the ground.
		/// Function called on every begin of contact between two fixtures.
		/// \param contact Contact object of the collision.
		virtual void BeginContact(b2Contact* contact) override;

		/// Handle the end of contact between the ground and an actor.
		/// Function called on every end of contact between two fixtures.
		/// \param contact Contact object of the collision.
		virtual void EndContact(b2Contact* contact) override;

		/// This is called after the solver is finished.
		/// This allows to inspect e.g. impulses.
		/// \param contact Structure containing data about the contact.
		/// \param impulse Resulting impulse.
		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

	private:
		FallingListener m_fallingListener;  ///< The falling collision listener.
		ArrowHitListener m_arrowHitListener;///< The arrow hit listener.

		/// Do various checks to determine if the contact should occurs or not.
		/// \param contact Structure containing data about the contact.
		/// \param oldManifold Old manifold.
		/// \return True if the contact should occurs, false otherwise.
		bool collide(b2Contact* contact, const b2Manifold* oldManifold);

		/// Do various checks just like \a collide but only on components and from A to B.
		/// \param entityA First entity.
		/// \param entityB Second entity.
		/// \return True if the contact should occurs, false otherwise.
		bool checkCollide(entityx::Entity entityA, entityx::Entity entityB) const;
};

#endif//CONTACTLISTENER_H
