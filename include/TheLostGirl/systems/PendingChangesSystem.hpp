#ifndef PENDINGCHANGESSYSTEM_HPP
#define PENDINGCHANGESSYSTEM_HPP

#include <queue>
#include <entityx/entityx.h>
#include <TheLostGirl/Command.hpp>

//Forward declarations
struct b2BodyDef;
class b2Body;
struct b2JointDef;
class b2Joint;

/// System that do things in the pending list (create bodies, destroy joints, apply actions, ...).
class PendingChangesSystem : public entityx::System<PendingChangesSystem>
{
	public:
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
};

#endif//PENDINGCHANGESSYSTEM_HPP
