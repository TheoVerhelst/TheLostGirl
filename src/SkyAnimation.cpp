#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/SkyAnimation.h>

SkyAnimation::SkyAnimation(entityx::Entity entity):
	m_entity(entity)
{}

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
			if(progress < 0.1875 or progress >= 0.8125)//Night
				daySpr.setColor(sf::Color(255, 255, 255, 0));
			else if(progress >= 0.1875 and progress < 0.3125)//Dawn
				daySpr.setColor(sf::Color(255, 255, 255, (progress - 0.1875) * 8 * 255));
			else if(progress >= 0.3125 and progress < 0.6875)//Day
				daySpr.setColor(sf::Color(255, 255, 255, 255));
			else if(progress >= 0.6875 and progress < 0.8125)//Twilight
				daySpr.setColor(sf::Color(255, 255, 255, (1 - (progress - 0.6875) * 8 * 255)));
			dayTrsf.angle = progress*360.f;
				
			//night sprite
			if(progress < 0.1875 or progress >= 0.8125)//Night
				nightSpr.setColor(sf::Color(255, 255, 255, 255));
			else if(progress >= 0.1875 and progress < 0.3125)//Dawn
				nightSpr.setColor(sf::Color(255, 255, 255, (1 - (progress - 0.1875) * 8 * 255)));
			else if(progress >= 0.3125 and progress < 0.6875)//Day
				nightSpr.setColor(sf::Color(255, 255, 255, 0));
			else if(progress >= 0.6875 and progress < 0.8125)//Twilight
				nightSpr.setColor(sf::Color(255, 255, 255, (progress - 0.6875) * 8 * 255));
			nightTrsf.angle = progress*360.f;
		}
	}
}
