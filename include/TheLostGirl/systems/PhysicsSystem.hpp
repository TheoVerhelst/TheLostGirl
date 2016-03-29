#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>

/// System that updates the Box2D's world and the drawables entities.
class PhysicsSystem : public entityx::System<PhysicsSystem>, private ContextAccessor<ContextElement::SystemManager,
                                                                                     ContextElement::Parameters,
                                                                                     ContextElement::World>
{
	public:
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif//PHYSICSSYSTEM_HPP
