#include <iostream>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/contactListeners/ActorIDListener.h>

ActorIDListener::ActorIDListener(State::Context context):
	m_context(context)
{}

void ActorIDListener::PreSolve(b2Contact *contact, const b2Manifold*)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	if(entityA->has_component<ActorIDComponent>() and entityB->has_component<ActorIDComponent>())
		//If the entities have the same ID
		if(entityA->component<ActorIDComponent>()->ID == entityB->component<ActorIDComponent>()->ID)
			contact->SetEnabled(false);
}

void ActorIDListener::BeginContact(b2Contact*)
{}

void ActorIDListener::EndContact(b2Contact*)
{}

void ActorIDListener::PostSolve(b2Contact*, const b2ContactImpulse*)
{}