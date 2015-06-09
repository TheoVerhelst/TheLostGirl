#include <iostream>
#include <deque>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/systems/RenderSystem.h>

RenderSystem::RenderSystem(StateStack::Context context):
	m_window(context.window),
	m_texture(context.postEffectsTexture),
	m_bloomEnabled(context.parameters.bloomEnabled),
	m_bloomEffect{},
	m_postEffectSupported{PostEffect::isSupported()}
{
}

void RenderSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	SpriteComponent::Handle spriteComponent;
	TransformComponent::Handle transformComponent;
	//A map containing all sprites grouped and sorted by z-order
	std::map<float, std::deque<const sf::Sprite*>> orderedEntities;
	//Add the sprites in the map
	for(entityx::Entity entity : entityManager.entities_with_components(spriteComponent, transformComponent))
	{
		const CategoryComponent::Handle categoryComponent(entity.component<CategoryComponent>());
		const bool scene{categoryComponent and categoryComponent->category & Category::Scene};
		for(const auto& spritePair : spriteComponent->sprites)
		{
			//If this is a scene entity, add its sprites beyond the others entities in this plan
			if(scene)
				orderedEntities[transformComponent->transforms.at(spritePair.first).z].push_front(&spritePair.second);
			else
				orderedEntities[transformComponent->transforms.at(spritePair.first).z].push_back(&spritePair.second);
		}
	}
	//Draw the texture and the GUI on the window and display
	if(m_postEffectSupported and m_bloomEnabled)
	{
		m_texture.clear({197, 182, 108});
		//For each plan, in the reverse order
		for(auto it(orderedEntities.crbegin()); it != orderedEntities.crend(); it++)
			//Draw the entities of this plan
			for(auto sprite : it->second)
				//Draw on the texture
				m_texture.draw(*sprite);
		m_texture.display();
		//Draw the texture on the window trough the bloom effect
		m_bloomEffect.apply(m_texture, m_window);
	}
	else
	{
		//For each plan, in the reverse order
		for(auto it(orderedEntities.crbegin()); it != orderedEntities.crend(); it++)
			//Draw the entities of this plan
			for(auto sprite : it->second)
				//Draw on the texture
				m_window.draw(*sprite);
	}

}
