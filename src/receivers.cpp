#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/receivers.h>

void ContactListener::PreSolve(b2Contact *contact, const b2Manifold*)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	if(entityA->has_component<ActorIDComponent>()
		and entityB->has_component<ActorIDComponent>())
	{
		//If the entities have the same ID
		if(entityA->component<ActorIDComponent>()->ID == entityB->component<ActorIDComponent>()->ID)
			contact->SetEnabled(false);
	}
}

void ContactListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Body* bodyA = fixtureA->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Fixture* fixtureB = contact->GetFixtureB();
	b2Body* bodyB = fixtureB->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	//If an actor A fall on the ground B, or if an actor B fall on the ground A
	if(		(entityA->has_component<CategoryComponent>()
			and entityB->has_component<FallComponent>()
			and entityA->component<CategoryComponent>()->category & Category::Ground)
		or (entityB->has_component<CategoryComponent>()
			and entityA->has_component<FallComponent>()
			and entityB->component<CategoryComponent>()->category & Category::Ground))
	{
		if(entityA->component<CategoryComponent>()->category & Category::Ground)
		{//B is an actor that fall on the ground A
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall on the ground B
		if(fixtureA->IsSensor())
		{
			if(entityA->has_component<AnimationsComponent>() and entityA->has_component<DirectionComponent>())
			{
				Animations* animations = entityA->component<AnimationsComponent>()->animations;
				animations->stop("fall right");
				animations->stop("fall left");
				if(entityA->has_component<JumpComponent>())
				{
					animations->stop("jump right");
					animations->stop("jump left");
				}
			}
			entityA->component<FallComponent>()->contactCount++;
			entityA->component<FallComponent>()->inAir = false;
		}
	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Body* bodyA = fixtureA->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Fixture* fixtureB = contact->GetFixtureB();
	b2Body* bodyB = fixtureB->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	//If an actor A fall from the ground B, or if an actor B fall from the ground A
	if(		(entityA->has_component<CategoryComponent>()
			and entityB->has_component<FallComponent>()
			and entityA->component<CategoryComponent>()->category & Category::Ground)
		or (entityB->has_component<CategoryComponent>()
			and entityA->has_component<FallComponent>()
			and entityB->component<CategoryComponent>()->category & Category::Ground))
	{
		if(entityA->component<CategoryComponent>()->category & Category::Ground)
		{//B is an actor that fall from the ground A
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall from the ground B
		if(fixtureA->IsSensor())
		{
			if(entityA->has_component<AnimationsComponent>() and
				entityA->has_component<DirectionComponent>() and
				not entityA->has_component<JumpComponent>())
			{
				Animations* animations = entityA->component<AnimationsComponent>()->animations;
				DirectionComponent::Handle directionComponent = entityA->component<DirectionComponent>();
				if(directionComponent->direction == Direction::Right)
					animations->play("fall right");
				else if(directionComponent->direction == Direction::Left)
					animations->play("fall left");
			}
			entityA->component<FallComponent>()->contactCount--;
			if(entityA->component<FallComponent>()->contactCount <= 0)
				entityA->component<FallComponent>()->inAir = true;
		}
	}
}