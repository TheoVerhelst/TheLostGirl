#include <iostream>

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
	m_scale(context.parameters.scale),
	m_levelRect{0, 0, 0, 0}
{}

void ScrollingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	if(m_levelRect != sf::IntRect(0, 0, 0, 0))
	{
		CategoryComponent::Handle categoryComponent;
		WalkComponent::Handle walkComponent;
		SpriteComponent::Handle spriteComponent;
		TransformComponent::Handle transformComponent;
		sf::Vector2f playerPosition;
		for(auto entity : entityManager.entities_with_components(transformComponent, categoryComponent, walkComponent))
		{
			//We found the player
			if(categoryComponent->category & Category::Player
				and transformComponent->transforms.find("main") != transformComponent->transforms.end())
			{
				//Find the main body
				playerPosition = {transformComponent->transforms.at("main").x, transformComponent->transforms.at("main").y};
				sf::View view{m_window.getView()};
				sf::Vector2f halfViewSize{view.getSize()/(m_scale*2.f)};
				//Compute the maximum and minimum center coordinates that the view can have
				sf::Vector2f viewCenterMin{sf::Vector2f(float(m_levelRect.left), float(m_levelRect.top))+halfViewSize};
				sf::Vector2f viewCenterMax{sf::Vector2f(float(m_levelRect.width + m_levelRect.left), float(m_levelRect.height + m_levelRect.top))-halfViewSize};
				//Cap the position between min and max
				playerPosition.x = cap(playerPosition.x, viewCenterMin.x, viewCenterMax.x);
				playerPosition.y = cap(playerPosition.y, viewCenterMin.y, viewCenterMax.y);

				//Assign the position to the view
				view.setCenter(playerPosition*m_scale);
				m_window.setView(view);

				//The position of the screen in world coordinates
				sf::Vector2f viewPos{playerPosition - viewCenterMin};
				//Assign transform on every sprite
				for(auto entitySecond : entityManager.entities_with_components(spriteComponent, transformComponent))
				{
					//For each transform in the map
					for(const auto& transformPair : transformComponent->transforms)
					{
						sf::Vector2f transform{transformPair.second.x, transformPair.second.y};
						//The abscissa of the entity on the screen, relatively to the reference plan and the position of the player
						sf::Vector2f scaledPos{transform + viewPos - (viewPos * std::pow(1.5f, m_referencePlan - transformPair.second.z))};
						spriteComponent->sprites.at(transformPair.first).setPosition(scaledPos.x*m_scale, scaledPos.y*m_scale);
						spriteComponent->sprites.at(transformPair.first).setRotation(transformPair.second.angle);
					}
				}
				break;
			}
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
}
