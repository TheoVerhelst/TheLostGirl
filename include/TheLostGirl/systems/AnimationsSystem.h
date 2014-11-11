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
/// \see Animations
class AnimationSystem : public entityx::System<AnimationSystem>
{
	public:
		/// Default constructor.
		AnimationSystem()
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif // ANIMATIONSSYSTEM_H