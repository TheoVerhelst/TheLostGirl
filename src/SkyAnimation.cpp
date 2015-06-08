#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/SkyAnimation.h>

SkyAnimation::SkyAnimation(entityx::Entity entity):
	m_entity(entity)
{
}

void SkyAnimation::animate(float progress)
{
	if(m_entity.has_component<SpriteComponent>() and m_entity.has_component<TransformComponent>())
	{
		SpriteComponent::Handle spriteComponent{m_entity.component<SpriteComponent>()};
		TransformComponent::Handle transformComponent{m_entity.component<TransformComponent>()};
		auto daySpr(spriteComponent->sprites.find("day"));
		auto nightSpr(spriteComponent->sprites.find("night"));
		auto dayTrsf(transformComponent->transforms.find("day"));
		auto nightTrsf(transformComponent->transforms.find("night"));
		if(daySpr != spriteComponent->sprites.end()
			and nightSpr != spriteComponent->sprites.end()
			and dayTrsf != transformComponent->transforms.end()
			and nightTrsf != transformComponent->transforms.end())
		{
			//day sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				daySpr->second.setColor(sf::Color::Transparent);
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				daySpr->second.setColor(sf::Color(255, 255, 255, sf::Uint8(255.f * 8.f * (progress - 0.1875f))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				daySpr->second.setColor(sf::Color::White);
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				daySpr->second.setColor(sf::Color(255, 255, 255, sf::Uint8( 255.f * (1 - 8.f * (progress - 0.6875f)))));
			dayTrsf->second.angle = progress*360.f;

			//night sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				nightSpr->second.setColor(sf::Color::White);
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				nightSpr->second.setColor(sf::Color(255, 255, 255, sf::Uint8(255.f * (1 - 8.f * (progress - 0.1875f)))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				nightSpr->second.setColor(sf::Color::Transparent);
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				nightSpr->second.setColor(sf::Color(255, 255, 255,sf::Uint8(8.f * 255.f * (progress - 0.6875f))));
			nightTrsf->second.angle = progress*360.f;
		}
	}
}
