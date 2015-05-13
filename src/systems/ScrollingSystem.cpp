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

ScrollingSystem::ScrollingSystem(sf::RenderWindow& window, Parameters& parameters):
	m_window(window),
	m_parameters(parameters),
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
				//Compute the maximum and minimum coordinates that the view can have
				float wiewWidth{view.getSize().x}, viewHeight{view.getSize().y};
				float xmin{m_levelRect.left + wiewWidth/(m_parameters.scale*2)};
				float xmax{m_levelRect.left + m_levelRect.width - wiewWidth/(m_parameters.scale*2)};
				float ymin{m_levelRect.top + viewHeight/(m_parameters.scale*2)};
				float ymax{m_levelRect.top + m_levelRect.height - viewHeight/(m_parameters.scale*2)};
				//Cap the position between min and max
				playerPosition.x = cap(playerPosition.x, xmin, xmax);
				playerPosition.y = cap(playerPosition.y, ymin, ymax);

				//Assign the position to the view
				view.setCenter(playerPosition*m_parameters.scale);
				m_window.setView(view);

				//The x-ordinate of the left border of the screen.
				float xScreen{playerPosition.x - xmin};
				//Assign transform on every sprite
				for(auto entitySecond : entityManager.entities_with_components(spriteComponent, transformComponent))
				{
					//For each transform in the map
					for(const auto& transformPair : transformComponent->transforms)
					{
						//The abscissa of the entity on the screen, relatively to the reference plan and the position of the player
						double xScaled{transformPair.second.x + xScreen - (xScreen * pow(1.5, m_referencePlan - transformPair.second.z))};
						spriteComponent->sprites.at(transformPair.first).setPosition(xScaled*m_parameters.scale, transformPair.second.y*m_parameters.scale);
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
