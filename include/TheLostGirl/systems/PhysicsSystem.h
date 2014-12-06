#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

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
		/// Default constructor.
		/// \param world b2World where the physics entities should be in.
		/// \param parameters Structure containing all the game parameters.
		/// \param systemManager The system manager of the game.
		PhysicsSystem(b2World& world, Parameters& parameters, entityx::SystemManager& systemManager):
			m_world(world),
			m_parameters(parameters),
			m_systemManager(systemManager)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;                       ///< b2World where the physics entities should be in.
		Parameters& m_parameters;               ///< Structure containing all the game parameters.
		entityx::SystemManager& m_systemManager;///< The system manager of the game.
};

#endif//PHYSICSSYSTEM_H
