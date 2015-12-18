#include <deque>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/PostEffect.h>
#include <TheLostGirl/systems/RenderSystem.h>

RenderSystem::RenderSystem():
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
		//If this is a scene entity, add its sprites beyond the others entities in this plan
		if(categoryComponent and categoryComponent->category & Category::Scene)
			orderedEntities[transformComponent->transform.z].push_front(&spriteComponent->sprite);
		else
			orderedEntities[transformComponent->transform.z].push_back(&spriteComponent->sprite);
	}
	//Draw sprites on the texture or on the window
	//For each plan, in the reverse order
	for(auto it(orderedEntities.crbegin()); it != orderedEntities.crend(); it++)
		//Draw the entities of this plan
		for(auto sprite : it->second)
			if(m_postEffectSupported and Context::parameters->bloomEnabled)
				Context::postEffectsTexture->draw(*sprite);
			else
				Context::window->draw(*sprite);

}
