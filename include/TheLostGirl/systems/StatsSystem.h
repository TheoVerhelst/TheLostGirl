#ifndef STATSSYSTEM_H
#define STATSSYSTEM_H

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that set the correct view according to the player position.
class StatsSystem : public entityx::System<StatsSystem>
{
	public:
		/// Default constructor.
		StatsSystem()
		{}
		
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif//STATSSYSTEM_H
