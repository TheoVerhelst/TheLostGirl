#include <iostream>
#include <deque>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>

#include <TheLostGirl/systems/RenderSystem.h>

void RenderSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	SpriteComponent::Handle spriteComponent;
	TransformComponent::Handle transformComponent;
	//A map containing all sprites grouped and sorted by z-order
	std::map<float, std::deque<sf::Sprite*>> orderedEntities;
	//Add the sprites in the map
	for(entityx::Entity entity : entityManager.entities_with_components(spriteComponent, transformComponent))
	{
		for(auto& spritePair : spriteComponent->sprites)
		{
			//If this is a scene entity, add them sprites beyond the others entities in this plan
			if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Scene)
				orderedEntities[transformComponent->transforms[spritePair.first].z].push_front(&spritePair.second);
			else
				orderedEntities[transformComponent->transforms[spritePair.first].z].push_back(&spritePair.second);
		}
	}
	//For each plan, in the reverse order
	for(std::multimap<float, std::deque<sf::Sprite*>>::const_reverse_iterator it{orderedEntities.crbegin()}; it != orderedEntities.crend(); it++)
		//Draw the entities of this plan
		for(auto sprite : it->second)
			m_window.draw(*sprite);
}
