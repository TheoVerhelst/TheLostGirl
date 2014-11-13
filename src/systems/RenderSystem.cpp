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
	//A map containing all entities grouped and sorted by z-order
	std::map<float, std::deque<entityx::Entity>> orderedEntities;
	//Add the entities in the map
	for(entityx::Entity entity : entityManager.entities_with_components(spriteComponent))
	{
		//If this is a scene entity, add it beyond the others entities in this plan
		if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Scene)
			orderedEntities[spriteComponent->plan].push_front(entity);
		else
			orderedEntities[spriteComponent->plan].push_back(entity);
	}
	//For each plan, in the reverse order
	for(std::multimap<float, std::deque<entityx::Entity>>::const_reverse_iterator it{orderedEntities.crbegin()}; it != orderedEntities.crend(); it++)
		//Draw the entities of this plan
		for(auto entity : it->second)
			//Draw every sprite of the entity
			for(auto& spritePair : entity.component<SpriteComponent>()->sprites)
				m_window.draw(spritePair.second);
}