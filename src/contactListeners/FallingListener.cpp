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
		if(fixtureHasRole(fixtureB, FixtureRole::Foot) and entityB.has_component<FallComponent>())
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall on the ground B
		if(fixtureHasRole(fixtureA, FixtureRole::Foot) and entityA.has_component<FallComponent>())
		{
			if(entityA.has_component<AnimationsComponent<SpriteSheetAnimation>>() and entityA.has_component<DirectionComponent>())
			{
				//Get all the animations managers of the entity
				auto& animationsManagers(entityA.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
				//For each animations manager of the entity
				for(auto& animationsPair : animationsManagers)
				{
					AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
					//If the animations manager have the required animation
					if(animations.isRegistred("fall left") and animations.isRegistred("fall right"))
					{
						animations.stop("fall left");
						animations.stop("fall right");
						if(entityA.has_component<JumpComponent>()
							and animations.isRegistred("jump left") and animations.isRegistred("jump right"))
						{
							animations.stop("jump left");
							animations.stop("jump right");
						}
					}
				}
			}
			entityA.component<FallComponent>()->contactCount++;
			entityA.component<FallComponent>()->inAir = false;
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
		if(fixtureHasRole(fixtureB, FixtureRole::Foot) and entityB.has_component<FallComponent>())
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		//Now we are sure that A is an actor that fall from the ground B
		if(fixtureHasRole(fixtureA, FixtureRole::Foot) and entityA.has_component<FallComponent>())
		{
			if(entityA.has_component<AnimationsComponent<SpriteSheetAnimation>>() and
				entityA.has_component<DirectionComponent>() and
				not entityA.has_component<JumpComponent>())
			{
				DirectionComponent::Handle directionComponent{entityA.component<DirectionComponent>()};
				//Get all the animations managers of the entity
				auto& animationsManagers(entityA.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
				//For each animations manager of the entity
				for(auto& animationsPair : animationsManagers)
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
			entityA.component<FallComponent>()->contactCount--;
			if(entityA.component<FallComponent>()->contactCount <= 0)
				entityA.component<FallComponent>()->inAir = true;
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
		if(entityA.has_component<HealthComponent>()
				and entityA.has_component<FallComponent>()
				and impulse->normalImpulses[0] > entityA.component<FallComponent>()->fallingResistance)
		{
			std::cout << "impact = " << (impulse->normalImpulses[0] - entityA.component<FallComponent>()->fallingResistance)*10.f << std::endl;
			entityA.component<HealthComponent>()->current -= (impulse->normalImpulses[0] - entityA.component<FallComponent>()->fallingResistance)*10.f;
		}

		if(entityB.has_component<HealthComponent>()
				and entityB.has_component<FallComponent>()
				and impulse->normalImpulses[0] > entityB.component<FallComponent>()->fallingResistance)
		{
			std::cout << "impact = " << (impulse->normalImpulses[0] - entityB.component<FallComponent>()->fallingResistance)*10.f << std::endl;
			entityB.component<HealthComponent>()->current -= (impulse->normalImpulses[0] - entityB.component<FallComponent>()->fallingResistance)*10.f;
		}
	}
}
