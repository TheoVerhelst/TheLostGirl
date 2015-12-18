#include <SFML/System/Time.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/SkyAnimation.h>
#include <TheLostGirl/systems/AnimationsSystem.h>

void AnimationsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent;
	AnimationsComponent<SkyAnimation>::Handle skyAnimationsComponent;
	SpriteComponent::Handle spriteComponent;
	FallComponent::Handle fallComponent;
	BodyComponent::Handle bodyComponent;
	DirectionComponent::Handle directionComponent;

	//For every entity that can fall, set the right animation
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															bodyComponent,
															directionComponent,
															fallComponent))
	{
		AnimationsManager<SpriteSheetAnimation>& animations(animationsComponent->animationsManager);
		if(fallComponent->inAir and bodyComponent->body->GetLinearVelocity().y > 2.f
			and animations.isRegistred("jump left") and animations.isRegistred("jump right"))
		{
			if(directionComponent->direction == Direction::Left)
				animations.stop("jump left");
			else if(directionComponent->direction == Direction::Right)
				animations.stop("jump right");
		}
	}

	//Update the AnimationsManager components
	for(auto entity : entityManager.entities_with_components(animationsComponent))
		animationsComponent->animationsManager.update(sf::seconds(float(dt)));

	//Update the AnimationsManager components of the sky entities
	for(auto entity : entityManager.entities_with_components(skyAnimationsComponent))
		skyAnimationsComponent->animationsManager.update(sf::seconds(float(dt)*m_timeSpeed));
}

void AnimationsSystem::setTimeSpeed(float timeSpeed)
{
	m_timeSpeed = timeSpeed;
}
