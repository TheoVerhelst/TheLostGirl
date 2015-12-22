#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/SkyAnimation.h>

SkyAnimation::SkyAnimation(entityx::Entity entity):
	m_entity(entity)
{
}

void SkyAnimation::animate(float progress)
{
	SpriteComponent::Handle spriteComponent{m_entity.component<SpriteComponent>()};
	TransformComponent::Handle transformComponent{m_entity.component<TransformComponent>()};
	const NameComponent::Handle nameComponent{m_entity.component<NameComponent>()};
	if(spriteComponent and transformComponent and nameComponent)
	{
		if(nameComponent->name == "day sky")
		{
			//day sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				spriteComponent->sprite.setColor(sf::Color::Transparent);
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				spriteComponent->sprite.setColor(sf::Color(255, 255, 255, sf::Uint8(255.f * 8.f * (progress - 0.1875f))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				spriteComponent->sprite.setColor(sf::Color::White);
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				spriteComponent->sprite.setColor(sf::Color(255, 255, 255, sf::Uint8( 255.f * (1 - 8.f * (progress - 0.6875f)))));
		}
		else if(nameComponent->name == "night sky")
		{
			//night sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				spriteComponent->sprite.setColor(sf::Color::White);
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				spriteComponent->sprite.setColor(sf::Color(255, 255, 255, sf::Uint8(255.f * (1 - 8.f * (progress - 0.1875f)))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				spriteComponent->sprite.setColor(sf::Color::Transparent);
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				spriteComponent->sprite.setColor(sf::Color(255, 255, 255,sf::Uint8(8.f * 255.f * (progress - 0.6875f))));
		}
		transformComponent->transform.angle = progress * 360.f;
	}
}
