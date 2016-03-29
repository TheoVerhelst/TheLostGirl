#ifndef ANIMATIONSSYSTEM_HPP
#define ANIMATIONSSYSTEM_HPP

#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include <TheLostGirl/Context.hpp>

/// System that plays animations on entities.
/// \see AnimationsManager
class AnimationsSystem : public entityx::System<AnimationsSystem>, private ContextAccessor<ContextElement::Window,
                                                                                           ContextElement::Parameters,
                                                                                           ContextElement::PostEffectsTexture,
                                                                                           ContextElement::SystemManager>
{
	public:
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

		/// Set the speed of the time.
		/// \param timeSpeed New value fot the speed of the time.
		void setTimeSpeed(float timeSpeed);

	private:
		float m_timeSpeed = 1.f;///< Current speed of the time.
};

#endif//ANIMATIONSSYSTEM_HPP
