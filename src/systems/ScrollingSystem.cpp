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
		entityx::Entity playerEntity;
		bool found{false};
		for(auto entity : entityManager.entities_with_components(bodyComponent,
																categoryComponent,
																walkComponent))
		{
			found = categoryComponent->category & Category::Player;
			if(found)
			{
				playerEntity = entity;
				break;
			}
		}
		if(found)
		{
			sf::Vector2f position = b2tosf(m_parameters.pixelScale * playerEntity.component<BodyComponent>()->body->GetPosition());
			sf::View view{m_window.getView()};
			//Compute the maximum and minimum coordinates that the view can have
			float xmin{m_levelRect.left*m_parameters.scale + (view.getSize().x / 2)};
			float xmax{m_levelRect.left*m_parameters.scale + m_levelRect.width*m_parameters.scale - (view.getSize().x / 2)};
			float ymin{m_levelRect.top*m_parameters.scale + (view.getSize().y / 2)};
			float ymax{m_levelRect.top*m_parameters.scale + m_levelRect.height*m_parameters.scale - (view.getSize().y / 2)};
			//Cap the position between min and max
			position.x = cap(position.x, xmin, xmax);
			position.y = cap(position.y, ymin, ymax);
			
			//Assign position on every entity Sprite
			for(auto entity : entityManager.entities_with_components(spriteComponent))
			{
				//The number of the layer where is the entity
				float z = spriteComponent->worldPosition.z;
				//The x-ordinate of the left border of the screen.
				float xScreen = position.x - xmin;
				//So the abscissa of the entity on the screen, relatively to the reference plan and the position of the player
				float xScaled = spriteComponent->worldPosition.x + xScreen - (xScreen * pow(1.5, m_referencePlan - z));
				spriteComponent->sprite->setPosition(xScaled, spriteComponent->worldPosition.y);
			}
			
			//Assign the position to the view
			view.setCenter(position);
			m_window.setView(view);
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
}