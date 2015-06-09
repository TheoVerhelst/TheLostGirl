#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/contactListeners/FallingListener.h>

FallingListener::FallingListener(StateStack::Context context):
	m_context(context)
{
}

void FallingListener::PreSolve(b2Contact *, const b2Manifold*)
{
}

void FallingListener::BeginContact(b2Contact* contact)
{
	b2Fixture* fixtureA{contact->GetFixtureA()};
	b2Body* bodyA{fixtureA->GetBody()};
	entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};
	b2Fixture* fixtureB{contact->GetFixtureB()};
	b2Body* bodyB{fixtureB->GetBody()};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};
	if(entityA != entityB)
	{
		//If B is an actor that fall on the ground A
		if(fixtureHasRole(fixtureB, FixtureRole::Foot))
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall on the ground B
		FallComponent::Handle fallComponent(entityA.component<FallComponent>());
		if(fixtureHasRole(fixtureA, FixtureRole::Foot) and fallComponent)
		{
			AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent(entityA.component<AnimationsComponent<SpriteSheetAnimation>>());
			if(animationsComponent)
			{
				//For each animations manager of the entity
				for(auto& animationsPair : animationsComponent->animationsManagers)
				{
					AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
					//If the animations manager have the required animation
					if(animations.isRegistred("fall left") and animations.isRegistred("fall right"))
					{
						animations.stop("fall left");
						animations.stop("fall right");
						if(animations.isRegistred("jump left") and animations.isRegistred("jump right"))
						{
							animations.stop("jump left");
							animations.stop("jump right");
						}
					}
				}
			}
			fallComponent->contactCount++;
			fallComponent->inAir = false;
		}
	}
}

void FallingListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA{contact->GetFixtureA()};
	b2Body* bodyA{fixtureA->GetBody()};
	entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};
	b2Fixture* fixtureB{contact->GetFixtureB()};
	b2Body* bodyB{fixtureB->GetBody()};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};
	if(entityA != entityB)
	{
		//If an actor A fall from the ground B, or if an actor B fall from the ground A
		if(fixtureHasRole(fixtureB, FixtureRole::Foot))
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall from the ground B
		FallComponent::Handle fallComponent(entityA.component<FallComponent>());
		if(fixtureHasRole(fixtureA, FixtureRole::Foot) and fallComponent)
		{
			AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent(entityA.component<AnimationsComponent<SpriteSheetAnimation>>());
			const DirectionComponent::Handle directionComponent(entityA.component<DirectionComponent>());
			if(animationsComponent and directionComponent)
			{
				//For each animations manager of the entity
				for(auto& animationsPair : animationsComponent->animationsManagers)
				{
					AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
					//If the animations manager have the required animation
					if(animations.isRegistred("fall left") and animations.isRegistred("fall right"))
					{
						if(directionComponent->direction == Direction::Left)
							animations.play("fall left");
						else if(directionComponent->direction == Direction::Right)
							animations.play("fall right");
					}
				}
			}
			fallComponent->contactCount--;
			if(fallComponent->contactCount == 0)
				fallComponent->inAir = true;
		}
	}
}

void FallingListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	//Handle falling damage
	entityx::Entity entityA{*static_cast<entityx::Entity*>(contact->GetFixtureA()->GetBody()->GetUserData())};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(contact->GetFixtureB()->GetBody()->GetUserData())};
	if(entityA != entityB)
	{
		//Sum the impact
		HealthComponent::Handle healthComponent(entityA.component<HealthComponent>());
		FallComponent::Handle fallComponent(entityA.component<FallComponent>());
		if(healthComponent and fallComponent and impulse->normalImpulses[0] > fallComponent->fallingResistance)
//		{
//			std::cout << "impact = " << (impulse->normalImpulses[0] - fallComponent->fallingResistance)*10.f << std::endl;
			healthComponent->current -= (impulse->normalImpulses[0] - fallComponent->fallingResistance)*10.f;
//		}

		healthComponent = entityB.component<HealthComponent>();
		fallComponent = entityB.component<FallComponent>();
		if(healthComponent and fallComponent and impulse->normalImpulses[0] > fallComponent->fallingResistance)
//		{
//			std::cout << "impact = " << (impulse->normalImpulses[0] - fallComponent->fallingResistance)*10.f << std::endl;
			healthComponent->current -= (impulse->normalImpulses[0] - fallComponent->fallingResistance)*10.f;
//		}
	}
}
