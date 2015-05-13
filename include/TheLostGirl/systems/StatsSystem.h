#ifndef STATSSYSTEM_H
#define STATSSYSTEM_H

#include <TheLostGirl/StateStack.h>

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
		/// \param context Current context of the application.
		StatsSystem(StateStack::Context context);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		StateStack::Context m_context;///< Current context of the application.
};

#endif//STATSSYSTEM_H
