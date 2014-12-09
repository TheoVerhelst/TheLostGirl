#ifndef ANIMATIONSSYSTEM_H
#define ANIMATIONSSYSTEM_H

#include <SFML/Graphics/Vertex.hpp>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
	class SystemManager;
}
class b2World;
struct Parameters;

/// System that plays animations on entities.
/// \see AnimationsManager
class AnimationsSystem : public entityx::System<AnimationsSystem>
{
	public:
		/// Default constructor.
		AnimationsSystem()
		{
		}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
		
		void setTimeSpeed(float timeSpeed);
	
	private:
		float m_timeSpeed;
};

#endif//ANIMATIONSSYSTEM_H
