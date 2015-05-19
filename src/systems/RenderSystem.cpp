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
	m_texture{},
	m_bloomEnabled(context.parameters.bloomEnabled),
	m_bloomEffect{},
	m_postEffectSupported{PostEffect::isSupported()}
{
	m_texture.create(m_window.getSize().x, m_window.getSize().y);
}

void RenderSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	SpriteComponent::Handle spriteComponent;
	TransformComponent::Handle transformComponent;
	//A map containing all sprites grouped and sorted by z-order
	std::map<float, std::deque<const sf::Sprite*>> orderedEntities;
	//Add the sprites in the map
	for(const entityx::Entity& entity : entityManager.entities_with_components(spriteComponent, transformComponent))
	{
		bool scene{entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Scene};
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
		const sf::View& windowView(m_window.getView());
		const sf::FloatRect viewRect(windowView.getCenter()-windowView.getSize()/2.f, windowView.getSize());
		m_texture.clear({255, 0, 0});
		//For each plan, in the reverse order
		for(auto it(orderedEntities.crbegin()); it != orderedEntities.crend(); it++)
			//Draw the entities of this plan
			for(auto sprite : it->second)
				//Draw on the texture
				m_texture.draw(*sprite);
		m_texture.display();
		m_texture.setView(windowView);
		//Draw the texture on the window trough the bloom effect
		m_bloomEffect.apply(m_texture, m_window, sf::Transform().translate(viewRect.left, viewRect.top));
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
