#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include <entityx/System.h>
#include <TheLostGirl/StateStack.h>

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
		/// Constructor.
		/// \param context Current context of the application.
		PhysicsSystem(StateStack::Context context);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;                       ///< b2World where the physics entities should be in.
		float& m_pixelByMeter;                  ///< Pixels/meter scale, not scaled.
		entityx::SystemManager& m_systemManager;///< The system manager of the game.
};

#endif//PHYSICSSYSTEM_H
