#include <iostream>

#include <SFML/System/Time.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/systems/AnimationsSystem.h>

void AnimationSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	AnimationsComponent<sf::Sprite>::Handle animationsComponent;
	SpriteComponent::Handle spriteComponent;
	FallComponent::Handle fallComponent;
	BodyComponent::Handle bodyComponent;
	DirectionComponent::Handle directionComponent;
	BendComponent::Handle bendComponent;
	
	//For every entity that can fall, set the right animation
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															bodyComponent,
															directionComponent,
															fallComponent))
	{
		std::map<std::string, b2Body*>& bodies = bodyComponent->bodies;
		for(auto& animationsPair : animationsComponent->animationsManagers)
		{
			Animations<sf::Sprite>& animations = animationsPair.second;
			if(bodies.find(animationsPair.first) != bodies.end())
			{
				b2Body* body = bodies[animationsPair.first];
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
	
	//Update the Animations components
	for(auto entity : entityManager.entities_with_components(animationsComponent, spriteComponent))
		//For each animations manager of the entity
		for(auto& animationsPair : animationsComponent->animationsManagers)
			//If the associated sprite exists
			if(spriteComponent->sprites.find(animationsPair.first) != spriteComponent->sprites.end())
				//Animate the sprite with the associed animations
				animationsPair.second.update(spriteComponent->sprites[animationsPair.first], sf::seconds(dt));
}