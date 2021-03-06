#ifndef STATSSYSTEM_HPP
#define STATSSYSTEM_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>

/// System that set the correct view according to the player position.
class StatsSystem : public entityx::System<StatsSystem>
{
	public:
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::SystemManager> Context;
};

#endif//STATSSYSTEM_HPP
