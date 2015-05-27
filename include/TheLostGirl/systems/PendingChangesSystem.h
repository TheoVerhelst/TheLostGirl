#ifndef PENDINGCHANGESSYSTEM_H
#define PENDINGCHANGESSYSTEM_H

#include <queue>
#include <entityx/System.h>

#include <TheLostGirl/StateStack.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
}
struct Command;
struct b2BodyDef;
class b2Body;
struct b2JointDef;
class b2Joint;
class b2World;

/// System that do things in the pending list (create bodies, destroy joints, apply actions, ...).
class PendingChangesSystem : public entityx::System<PendingChangesSystem>
{
	public:
		/// Constructor.
		/// \param context Current context of the application.
		PendingChangesSystem(StateStack::Context context);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

		std::queue<Command> commandQueue;      ///< List of all commands that need to be executed.
		std::queue<b2BodyDef*> bodiesToCreate; ///< List of all bodies that need to be created.
		std::queue<b2Body*> bodiesToDestroy;   ///< List of all bodies that need to be destroyed.
		std::queue<b2JointDef*> jointsToCreate;///< List of all joints that need to be created.
		std::queue<b2Joint*> jointsToDestroy;  ///< List of all joints that need to be destroyed.

	private:
		b2World& m_world;///< Reference to the current world of the game.
};

#endif//PENDINGCHANGESSYSTEM_H
