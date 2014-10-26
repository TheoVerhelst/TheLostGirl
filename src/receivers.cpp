#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/receivers.h>

void FallingListener::BeginContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	if(entityA->has_component<CategoryComponent>() and entityB->has_component<CategoryComponent>())
	{
		if(entityB->component<CategoryComponent>()->category & Category::CanFall and entityA->component<CategoryComponent>()->category & Category::Ground)
		{//B is an actor that fall on the ground A
			//Swap the pointers
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		CategoryComponent::Handle categoryComponentA = entityA->component<CategoryComponent>();
		CategoryComponent::Handle categoryComponentB = entityB->component<CategoryComponent>();
		if(categoryComponentA->category & Category::CanFall and categoryComponentB->category & Category::Ground)
		{//A is an actor that fall on the ground B
			if(entityA->has_component<AnimationsComponent>() and entityA->has_component<DirectionComponent>())
			{
				Animations* animations = entityA->component<AnimationsComponent>()->animations;
				animations->stop("fallRight");
				animations->stop("fallLeft");
				if(entityA->has_component<Jump>())
				{
					animations->stop("jumpRight");
					animations->stop("jumpLeft");
				}
			}
			FallComponent::Handle fallComponent = entityA->component<FallComponent>();
			fallComponent->inAir = false;
		}
	}
}

void FallingListener::EndContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	if(entityA->has_component<CategoryComponent>() and entityB->has_component<CategoryComponent>())
	{
		if(entityB->component<CategoryComponent>()->category & Category::CanFall and entityA->component<CategoryComponent>()->category & Category::Ground)
		{//B is an actor that fall on the ground A
			//Swap the pointers
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		CategoryComponent::Handle categoryComponentA = entityA->component<CategoryComponent>();
		CategoryComponent::Handle categoryComponentB = entityB->component<CategoryComponent>();
		if(categoryComponentA->category & Category::CanFall and categoryComponentB->category & Category::Ground)
		{//A is an actor that fall on the ground B
			if(entityA->has_component<AnimationsComponent>() and
				entityA->has_component<DirectionComponent>() and
				not entityA->has_component<Jump>())
			{
				Animations* animations = entityA->component<AnimationsComponent>()->animations;
				DirectionComponent::Handle directionComponent = entityA->component<DirectionComponent>();
				if(directionComponent->direction == Direction::Right)
					animations->play("fallRight");
				else if(directionComponent->direction == Direction::Left)
					animations->play("fallLeft");
			}
			FallComponent::Handle fallComponent = entityA->component<FallComponent>();
			fallComponent->inAir = true;
		}
	}
}