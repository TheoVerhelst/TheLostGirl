#ifndef RECEIVERS_H
#define RECEIVERS_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>

//struct Score : public entityx::Receiver<Score>
//{
//	Score(entityx::Entity _entity, unsigned int initialScore = 0):
//		entity(_entity),
//		count(initialScore)
//	{}
//	
//	void receive(const WonGame &wonGame)
//	{
//		if(wonGame.winner == entity)
//			count++;
//	}
//	entityx::Entity entity;
//	unsigned int count;
//};

/// Ground collision listener.
/// Handle collisions between actors and the ground
/// to set the right animation and update the FallComponent.
class FallingListener : public b2ContactListener
{
	public:
		/// Handle the hitting of an actor with the ground.
		/// Function called on every begin of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void BeginContact(b2Contact* contact);
		
		///Handle the end of contact between the ground and an actor.
		/// Function called on every end of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void EndContact(b2Contact* contact);
};

#endif // RECEIVERS_H