#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2World.h>
#include <entityx/Entity.h>
#include <entityx/System.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

#include <TheLostGirl/contactListeners/ArrowHitListener.h>

ArrowHitListener::ArrowHitListener(StateStack::Context context):
	m_context(context)
{
}

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
		if(entityB.has_component<ArrowComponent>() and entityA.has_component<HardnessComponent>())
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		if(entityA.has_component<ArrowComponent>() and entityB.has_component<HardnessComponent>())
		{
			//If the impact multiplied by the penetrance of the arrow is greater than the hardness of the other object
			if(entityA.component<ArrowComponent>()->state == ArrowComponent::Fired and
				impulse->normalImpulses[0]*entityA.component<ArrowComponent>()->penetrance > entityB.component<HardnessComponent>()->hardness)
			{
				b2Vec2 localStickPoint{sftob2(entityA.component<ArrowComponent>()->localStickPoint/m_context.parameters.pixelByMeter)};
				b2Vec2 globalStickPoint{bodyA->GetWorldPoint(localStickPoint)};
				b2WeldJointDef* weldJointDef = new b2WeldJointDef();
				weldJointDef->bodyA = bodyA;
				weldJointDef->bodyB = bodyB;
				weldJointDef->localAnchorA = localStickPoint;
				weldJointDef->localAnchorB = bodyB->GetLocalPoint(globalStickPoint);
				weldJointDef->referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
				m_context.systemManager.system<PendingChangesSystem>()->jointsToCreate.push(weldJointDef);
				entityA.component<ArrowComponent>()->state = ArrowComponent::Sticked;
			}
			if(entityB.has_component<HealthComponent>())
				entityB.component<HealthComponent>()->current -= impulse->normalImpulses[0]*entityA.component<ArrowComponent>()->damage;
		}
	}
}
