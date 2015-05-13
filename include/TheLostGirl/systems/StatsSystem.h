#ifndef STATSSYSTEM_H
#define STATSSYSTEM_H

#include <queue>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}
struct Command;

/// System that set the correct view according to the player position.
class StatsSystem : public entityx::System<StatsSystem>
{
	public:
		/// Default constructor.
		/// \param commandQueue Queue of command where the actions should be putted in.
		StatsSystem(std::queue<Command>& commandQueue);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		std::queue<Command>& m_commandQueue;///< Queue of command where the actions should be putted in.
};

#endif//STATSSYSTEM_H
