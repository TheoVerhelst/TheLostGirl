#include <iostream>

#include <SFML/System/Time.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/systems/AnimationsSystem.h>

void AnimationSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	AnimationsComponent::Handle animationsComponent;
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
		Animations* animations = animationsComponent->animations;
		b2Body* body = bodyComponent->body;
		if(fallComponent->inAir and body->GetLinearVelocity().y > 2.f)
		{
			if(directionComponent->direction == Direction::Left)
				animations->play("fall left");
			else if(directionComponent->direction == Direction::Right)
				animations->play("fall right");
		}
	}
	
	//Update the Animations components
	for(auto entity : entityManager.entities_with_components(animationsComponent))
		animationsComponent->animations->update(entity, sf::seconds(dt));
}