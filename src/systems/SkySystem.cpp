#include <iostream>

#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/systems/SkySystem.h>

void SkySystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double spentTime)
{
	SkyComponent::Handle skyComponent;
	SpriteComponent::Handle spriteComponent;
	spentTime = remainder(spentTime, 600.f);//Get elapsed time since midnight
	
	for(auto entity : entityManager.entities_with_components(skyComponent, spriteComponent))
	{
		spriteComponent->sprite->setRotation(spentTime*0.6f);
		if(skyComponent->day)
		{
			if(spentTime < 112.5 or spentTime >= 487.5)//Night
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, 0));
			else if(spentTime >= 112.5 and spentTime < 187.5)//Dawn
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, ((spentTime - 112.5) / 75) * 255));
			else if(spentTime >= 187.5 and spentTime < 412.5)//Day
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, 255));
			else if(spentTime >= 412.5 and spentTime < 487.5)//Twilight
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, (1 - ((spentTime - 412.5) / 75)) * 255));
		}
		else
		{
			if(spentTime < 112.5 or spentTime >= 487.5)//Night
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, 255));
			else if(spentTime >= 112.5 and spentTime < 187.5)//Dawn
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, (1 - ((spentTime - 112.5) / 75)) * 255));
			else if(spentTime >= 187.5 and spentTime < 412.5)//Day
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, 0));
			else if(spentTime >= 412.5 and spentTime < 487.5)//Twilight
				spriteComponent->sprite->setColor(sf::Color(255, 255, 255, ((spentTime - 412.5) / 75) * 255));
		}
	}
}