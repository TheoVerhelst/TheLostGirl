#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <entityx/System.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}
class b2World;
struct Parameters;

/// System that updates the Box2D's world and the drawables entities.
class PhysicsSystem : public entityx::System<PhysicsSystem>
{
	public:
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif//PHYSICSSYSTEM_H
