#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/systems/ScrollingSystem.h>

ScrollingSystem::ScrollingSystem(StateStack::Context context):
	m_window(context.window),
	m_texture(context.postEffectsTexture),
	m_bloomEnabled(context.parameters.bloomEnabled),
	m_scale(context.parameters.scale),
	m_levelRect{0, 0, 0, 0}
{}

void ScrollingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	if(m_levelRect != sf::IntRect(0, 0, 0, 0) and m_player.valid())
	{
		TransformComponent::Handle transformComponent(m_player.component<TransformComponent>());
		sf::Vector2f playerPosition{transformComponent->transform.x, transformComponent->transform.y};
		sf::View view{m_bloomEnabled ? m_texture.getView() : m_window.getView()};
		sf::Vector2f halfViewSize{view.getSize()/(m_scale*2.f)};
		//Compute the maximum and minimum center coordinates that the view can have
		sf::Vector2f viewCenterMin{sf::Vector2f(float(m_levelRect.left), float(m_levelRect.top))+halfViewSize};
		sf::Vector2f viewCenterMax{sf::Vector2f(float(m_levelRect.width + m_levelRect.left), float(m_levelRect.height + m_levelRect.top))-halfViewSize};
		//Cap the position between min and max
		playerPosition.x = cap(playerPosition.x, viewCenterMin.x, viewCenterMax.x);
		playerPosition.y = cap(playerPosition.y, viewCenterMin.y, viewCenterMax.y);

		//Assign the position to the view
		view.setCenter(playerPosition*m_scale);
		if(m_bloomEnabled)
			m_texture.setView(view);
		else
			m_window.setView(view);

		//The position of the screen in world coordinates
		const sf::Vector2f viewPos{playerPosition - viewCenterMin};
		//Assign transform on every sprite
		SpriteComponent::Handle spriteComponent;
		for(auto entitySecond : entityManager.entities_with_components(spriteComponent, transformComponent))
		{
			const Transform& transform(transformComponent->transform);
			//The abscissa of the entity on the screen, relatively to the reference plan and the position of the player
			const sf::Vector2f scaledPos{{transform.x, transform.y} + viewPos - (viewPos * std::pow(1.5f, m_referencePlan - transform.z))};
			spriteComponent->sprites.at(transformPair.first).setPosition(scaledPos.x*m_scale, scaledPos.y*m_scale);
			spriteComponent->sprites.at(transformPair.first).setRotation(transform.angle);
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
}

void ScrollingSystem::searchPlayer(entityx::EntityManager& entityManager)
{
	CategoryComponent::Handle categoryComponent;
	TransformComponent::Handle transformComponent;
	for(auto entity : entityManager.entities_with_components(categoryComponent))
	{
		//We found the player
		if(categoryComponent->category & Category::Player)
		{
			m_player = entity;
			break;
		}
	}
}
