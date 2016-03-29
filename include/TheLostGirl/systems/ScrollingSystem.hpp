#ifndef SCROLLINGSYSTEM_HPP
#define SCROLLINGSYSTEM_HPP

#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include <TheLostGirl/Context.hpp>

/// System that set the correct view according to the player position.
class ScrollingSystem : public entityx::System<ScrollingSystem>, private ContextAccessor<ContextElement::Window,
                                                                                         ContextElement::Parameters,
                                                                                         ContextElement::PostEffectTexture,
                                                                                         ContextElement::EntityManager,
                                                                                         ContextElement::SystemManager>
{
	public:
		/// Constructor.
		ScrollingSystem();

		/// Set the level bounds. This must be called before the first update.
		/// \param levelRect Rectangle defining the level bounds.
		/// \param referencePlan Number of the plan where actors evolute.
		void setLevelData(const sf::FloatRect& levelRect, float referencePlan);

		/// Search the player in all entities, in order to avoid unnecessary search at each update.
		/// This function must be called every time the player's entity change, such as at the beginning of a level.
		void searchPlayer();

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		entityx::Entity m_player; ///< The player entity, it is fulfilled with searchPlayer().
		sf::FloatRect m_levelRect;///< Rectangle defining the level bounds.
		float m_referencePlan;    ///< Number of the plan where actors evolute.
};

#endif//SCROLLINGSYSTEM_HPP
