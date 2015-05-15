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
		if(spriteComponent->sprites.find("day") != spriteComponent->sprites.end()
			and spriteComponent->sprites.find("night") != spriteComponent->sprites.end()
			and transformComponent->transforms.find("day") != transformComponent->transforms.end()
			and transformComponent->transforms.find("night") != transformComponent->transforms.end())
		{
			sf::Sprite& daySpr(spriteComponent->sprites["day"]);
			sf::Sprite& nightSpr(spriteComponent->sprites["night"]);
			Transform& dayTrsf(transformComponent->transforms["day"]);
			Transform& nightTrsf(transformComponent->transforms["night"]);

			//day sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				daySpr.setColor(sf::Color(255, 255, 255, 0));
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				daySpr.setColor(sf::Color(255, 255, 255, sf::Uint8(8 * 255 * sf::Uint8(progress - 0.1875f))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				daySpr.setColor(sf::Color(255, 255, 255, 255));
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				daySpr.setColor(sf::Color(255, 255, 255, sf::Uint8(1 - 8 * 255 * sf::Uint8(progress - 0.6875f))));
			dayTrsf.angle = progress*360.f;

			//night sprite
			if(progress < 0.1875f or progress >= 0.8125f)//Night
				nightSpr.setColor(sf::Color(255, 255, 255, 255));
			else if(progress >= 0.1875f and progress < 0.3125f)//Dawn
				nightSpr.setColor(sf::Color(255, 255, 255, sf::Uint8(1 - 8 * 255 * sf::Uint8(progress - 0.1875f))));
			else if(progress >= 0.3125f and progress < 0.6875f)//Day
				nightSpr.setColor(sf::Color(255, 255, 255, 0));
			else if(progress >= 0.6875f and progress < 0.8125f)//Twilight
				nightSpr.setColor(sf::Color(255, 255, 255,sf::Uint8( 8 * 255 * sf::Uint8(progress - 0.6875f))));
			nightTrsf.angle = progress*360.f;
		}
	}
}
