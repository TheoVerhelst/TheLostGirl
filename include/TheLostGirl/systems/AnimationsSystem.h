#ifndef ANIMATIONSSYSTEM_H
#define ANIMATIONSSYSTEM_H

#include <entityx/System.h>

#include <SFML/Graphics/Vertex.hpp>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that plays animations on entities.
/// \see AnimationsManager
class AnimationsSystem : public entityx::System<AnimationsSystem>
{
	public:
		/// Constructor.
		/// \param context Current context of the application.
		AnimationsSystem(StateStack::Context context);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

		/// Set the speed of the time.
		/// \param timeSpeed New value fot the speed of the time.
		void setTimeSpeed(float timeSpeed);

	private:
		float m_timeSpeed;///< Current speed of the time.
};

#endif//ANIMATIONSSYSTEM_H
