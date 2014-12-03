#include <iostream>
#include <utility> //std::swap

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2World.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>

#include <TheLostGirl/contactListeners/ArrowHitListener.h>

ArrowHitListener::ArrowHitListener(State::Context context):
	m_context(context)
{}

void ArrowHitListener::PreSolve(b2Contact *, const b2Manifold*)
{}

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
		if(entityB.has_component<WindFrictionComponent>())
		{
			//Swap the pointers
			std::swap(fixtureA, fixtureB);
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		if(entityA.has_component<WindFrictionComponent>())
		{
			b2Vec2 worldAnchorPoint = bodyA->GetWorldPoint(b2Vec2(0.4f, 0.05416667));
			b2WeldJointDef weldJointDef;
			weldJointDef.bodyA = bodyA;
			weldJointDef.bodyB = bodyB;
			weldJointDef.localAnchorA = bodyA->GetLocalPoint(worldAnchorPoint);
			weldJointDef.localAnchorB = bodyB->GetLocalPoint(worldAnchorPoint);
			weldJointDef.referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
//			m_context.world.CreateJoint(&weldJointDef);
		}
	}
}
