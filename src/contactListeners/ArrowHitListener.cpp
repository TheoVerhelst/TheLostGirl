#include <utility> //std::swap
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/AnimationsManager.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>
#include <TheLostGirl/contactListeners/ArrowHitListener.hpp>

void ArrowHitListener::PreSolve(b2Contact *, const b2Manifold*)
{
}

void ArrowHitListener::BeginContact(b2Contact*)
{
}

void ArrowHitListener::EndContact(b2Contact*)
{
}

void ArrowHitListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	b2Fixture* fixtureA{contact->GetFixtureA()};
	b2Body* bodyA{fixtureA->GetBody()};
	entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};
	b2Fixture* fixtureB{contact->GetFixtureB()};
	b2Body* bodyB{fixtureB->GetBody()};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};
	if(entityA != entityB)
	{
		if(entityB.has_component<ArrowComponent>())
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		ArrowComponent::Handle arrowComponent(entityA.component<ArrowComponent>());
		HardnessComponent::Handle hardnessComponent(entityB.component<HardnessComponent>());
		if(arrowComponent and hardnessComponent)
		{
			//If the impact multiplied by the penetrance of the arrow is greater than the hardness of the other object
			if(arrowComponent->state == ArrowComponent::Fired and
				impulse->normalImpulses[0]*arrowComponent->penetrance > hardnessComponent->hardness)
			{
				b2Vec2 localStickPoint{sftob2(arrowComponent->localStickPoint/Context::getParameters().pixelByMeter)};
				b2Vec2 globalStickPoint{bodyA->GetWorldPoint(localStickPoint)};
				b2WeldJointDef* weldJointDef = new b2WeldJointDef();
				weldJointDef->bodyA = bodyA;
				weldJointDef->bodyB = bodyB;
				weldJointDef->localAnchorA = localStickPoint;
				weldJointDef->localAnchorB = bodyB->GetLocalPoint(globalStickPoint);
				weldJointDef->referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
				Context::getSystemManager().system<PendingChangesSystem>()->jointsToCreate.push(weldJointDef);
				arrowComponent->state = ArrowComponent::Sticked;
			}
			HealthComponent::Handle healthComponent(entityB.component<HealthComponent>());
			ActorComponent::Handle actorComponent(entityB.component<ActorComponent>());
			if(healthComponent and actorComponent)
			{
				float damages{impulse->normalImpulses[0]*4};
                damages *= arrowComponent->damage;
                damages -= actorComponent->arrowResistance;
                if(arrowComponent->shooter and arrowComponent->shooter.has_component<ArcherComponent>())
					damages += arrowComponent->shooter.component<ArcherComponent>()->damages;
				healthComponent->current -= damages;
			}
		}
	}
}
