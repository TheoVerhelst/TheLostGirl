#ifndef RECEIVERS_H
#define RECEIVERS_H

#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/b2Body.h>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

//struct Score : public entityx::Receiver<Score>
//{
//	Score(entityx::Entity _entity, unsigned int initialScore = 0):
//		entity(_entity),
//		count(initialScore)
//	{}
//	
//	void receive(const WonGame &wonGame)
//	{
//		if(wonGame.winner == entity)
//			count++;
//	}
//	entityx::Entity entity;
//	unsigned int count;
//};

/// Ground collision listener.
/// Handle collisions between actors and the ground
/// to set the right animation and update the FallComponent.
class FallingListener : public b2ContactListener
{
	public:
		/// Handle the hitting of an actor with the ground.
		/// Function called on every begin of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void BeginContact(b2Contact* contact)
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
		
		///Handle the end of contact between the ground and an actor.
		/// Function called on every end of contact between two fixtures.
		/// \param contact Contact object of the collision.
		void EndContact(b2Contact* contact)
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
};

#endif // RECEIVERS_H