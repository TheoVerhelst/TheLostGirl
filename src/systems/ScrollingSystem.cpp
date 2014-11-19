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

void ScrollingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	if(m_levelRect != sf::IntRect(0, 0, 0, 0))
	{
		BodyComponent::Handle bodyComponent;
		CategoryComponent::Handle categoryComponent;
		WalkComponent::Handle walkComponent;
		SpriteComponent::Handle spriteComponent;
		sf::Vector2f playerPosition;
		bool found{false};
		for(auto entity : entityManager.entities_with_components(bodyComponent,
																categoryComponent,
																walkComponent))
		{
			//We found the player
			if(categoryComponent->category & Category::Player)
			{
				//Find the main body
				std::map<std::string, b2Body*>& bodies(entity.component<BodyComponent>()->bodies);
				if(not bodies.empty())
				{
					//If there is a main body
					if(bodies.find("main") != bodies.end())
						playerPosition = b2tosf(bodies["main"]->GetPosition());
					//Else, take the first that come in hand
					else
						playerPosition = b2tosf(bodies.begin()->second->GetPosition());
				}
				found = true;
				break;
			}
		}
		if(found)
		{
			playerPosition *= m_parameters.pixelScale;
			sf::View view{m_window.getView()};
			//Compute the maximum and minimum coordinates that the view can have
			float xmin{m_levelRect.left*m_parameters.scale + (view.getSize().x / 2)};
			float xmax{m_levelRect.left*m_parameters.scale + m_levelRect.width*m_parameters.scale - (view.getSize().x / 2)};
			float ymin{m_levelRect.top*m_parameters.scale + (view.getSize().y / 2)};
			float ymax{m_levelRect.top*m_parameters.scale + m_levelRect.height*m_parameters.scale - (view.getSize().y / 2)};
			//Cap the position between min and max
			playerPosition.x = cap(playerPosition.x, xmin, xmax);
			playerPosition.y = cap(playerPosition.y, ymin, ymax);
			
			//Assign the position to the view
			view.setCenter(playerPosition);
			m_window.setView(view);
			
			//Assign position on every sprite
			for(auto entity : entityManager.entities_with_components(spriteComponent))
			{
				//The x-ordinate of the left border of the screen.
				float xScreen = playerPosition.x - xmin;
				std::map<std::string, sf::Sprite>& sprites(spriteComponent->sprites);
				std::map<std::string, sf::Vector3f>& worldPositions(spriteComponent->worldPositions);
				//For each position in the map
				for(auto& positionPair : worldPositions)
				{
					//If the associated sprite exists
					if(sprites.find(positionPair.first) != sprites.end())
					{
						//The abscissa of the entity on the screen, relatively to the reference plan and the position of the player
						float xScaled = positionPair.second.x + xScreen - (xScreen * pow(1.5, m_referencePlan - positionPair.second.z));
						sprites[positionPair.first].setPosition(xScaled, positionPair.second.y);
					}
				}
			}
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
}
