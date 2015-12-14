#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/systems/ScrollingSystem.h>
#include <TheLostGirl/Player.h>

ScrollingSystem::ScrollingSystem():
	m_levelRect{0, 0, 0, 0}
{}

void ScrollingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	if(m_levelRect != sf::IntRect(0, 0, 0, 0) and m_player.valid())
	{
		TransformComponent::Handle transformComponent(m_player.component<TransformComponent>());
		sf::Vector2f playerPosition{transformComponent->transform.x, transformComponent->transform.y};
		sf::View view{Context::parameters->bloomEnabled ? Context::postEffectsTexture->getView() : Context::window->getView()};
		const sf::Vector2f halfViewSize{view.getSize()/2.f};
		//Compute the maximum and minimum center coordinates that the view can have
		const sf::Vector2f viewCenterMin{sf::Vector2f(float(m_levelRect.left), float(m_levelRect.top))+halfViewSize};
		const sf::Vector2f viewCenterMax{sf::Vector2f(float(m_levelRect.width + m_levelRect.left), float(m_levelRect.height + m_levelRect.top))-halfViewSize};
		//Cap the position between min and max
		playerPosition.x = cap(playerPosition.x, viewCenterMin.x, viewCenterMax.x);
		playerPosition.y = cap(playerPosition.y, viewCenterMin.y, viewCenterMax.y);

		//Assign the position to the view
		view.setCenter(playerPosition);
		if(Context::parameters->bloomEnabled)
			Context::postEffectsTexture->setView(view);
		else
			Context::window->setView(view);

		//The position of the screen in world coordinates
		const sf::Vector2f viewPos{playerPosition - viewCenterMin};
		//Assign transform on every sprite
		SpriteComponent::Handle spriteComponent;
		for(auto entitySecond : entityManager.entities_with_components(spriteComponent, transformComponent))
		{
			const Transform& transform(transformComponent->transform);
			//The abscissa of the entity on the screen, relatively to the reference plan and the position of the player
			const sf::Vector2f scaledPos{sf::Vector2f(transform.x, transform.y) + viewPos - (viewPos * std::pow(1.5f, m_referencePlan - transform.z))};
			spriteComponent->sprite.setPosition(scaledPos);
			spriteComponent->sprite.setRotation(transform.angle);
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
}

void ScrollingSystem::searchPlayer()
{
	CategoryComponent::Handle categoryComponent;
	TransformComponent::Handle transformComponent;
	for(auto entity : Context::entityManager->entities_with_components(categoryComponent, transformComponent))
	{
		//We found the player
		if(categoryComponent->category & Category::Player)
		{
			m_player = entity;
			break;
		}
	}
}
