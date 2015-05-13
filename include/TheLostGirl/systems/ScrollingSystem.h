#ifndef SCROLLINGSYSTEM_H
#define SCROLLINGSYSTEM_H

#include <entityx/System.h>

#include <TheLostGirl/StateStack.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
}
struct Parameters;

/// System that set the correct view according to the player position.
class ScrollingSystem : public entityx::System<ScrollingSystem>
{
	public:
		/// Default constructor.
		/// \param context Current context of the application.
		ScrollingSystem(StateStack::Context context);

		/// Set the level bounds. This must be called before the first update.
		/// \param levelRect Rectangle defining the level bounds.
		/// \param referencePlan Number of the plan where actors evolute.
		void setLevelData(const sf::IntRect& levelRect, float referencePlan);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;///< SFML's window of wich set the view.
		float& m_scale;            ///< Current scale of graphics and  images.
		sf::IntRect m_levelRect;   ///< Rectangle defining the level bounds.
		float m_referencePlan;     ///< Number of the plan where actors evolute.
};

#endif//SCROLLINGSYSTEM_H
