#ifndef PENDINGCHANGESSYSTEM_H
#define PENDINGCHANGESSYSTEM_H

#include <queue>

#include <SFML/Graphics/Vertex.hpp>

#include <TheLostGirl/Command.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
	class SystemManager;
}
struct b2BodyDef;
class b2Body;
struct b2JointDef;
class b2Joint;
class b2World;

/// System that do things in the pending list (create bodies, destroy joints, apply actions, ...)
class PendingChangesSystem : public entityx::System<PendingChangesSystem>
{
	public:
		/// Default constructor.
		/// \param world The Box2D world.
		PendingChangesSystem(b2World& world):
			m_world(world)
		{
		}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
		
		std::queue<Command> commandQueue;
		std::queue<b2BodyDef*> bodiesToCreate;
		std::queue<b2Body*> bodiesToDestroy;
		std::queue<b2JointDef*> jointsToCreate;
		std::queue<b2Joint*> jointsToDestroy;
	
	private:
		b2World& m_world;
};

#endif//PENDINGCHANGESSYSTEM_H
