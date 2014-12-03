#include <iostream>

#include <SFML/System/Time.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
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
		std::map<std::string, b2Body*>& bodies(bodyComponent->bodies);
		for(auto& animationsPair : animationsComponent->animationsManagers)
		{
			AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
			if(bodies.find(animationsPair.first) != bodies.end())
			{
				b2Body* body{bodies[animationsPair.first]};
				if(fallComponent->inAir and body->GetLinearVelocity().y > 2.f
					and animations.isRegistred("fall left") and animations.isRegistred("fall right"))
				{
					if(directionComponent->direction == Direction::Left)
						animations.play("fall left");
					else if(directionComponent->direction == Direction::Right)
						animations.play("fall right");
				}
			}
		}
	}
	
	//Update the AnimationsManager components
	for(auto entity : entityManager.entities_with_components(animationsComponent))
		//For each animations manager of the entity
		for(auto& animationsPair : animationsComponent->animationsManagers)
			animationsPair.second.update(sf::seconds(dt));
	
	//Update the AnimationsManager components
	for(auto entity : entityManager.entities_with_components(skyAnimationsComponent))
		//For each animations manager of the entity
		for(auto& animationsPair : skyAnimationsComponent->animationsManagers)
			animationsPair.second.update(sf::seconds(dt*m_timeSpeed));
}

void AnimationsSystem::setTimeSpeed(float timeSpeed)
{
	m_timeSpeed = timeSpeed;
}
