#include <iostream>

#include <TheLostGirl/ContactListener.h>

ContactListener::ContactListener():
	m_fallingListener(),
	m_actorIDListener()
{}


void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	m_fallingListener.PreSolve(contact, oldManifold);
	m_actorIDListener.PreSolve(contact, oldManifold);
}

void ContactListener::BeginContact(b2Contact* contact)
{
	m_fallingListener.BeginContact(contact);
	m_actorIDListener.BeginContact(contact);
}

void ContactListener::EndContact(b2Contact* contact)
{
	m_fallingListener.EndContact(contact);
	m_actorIDListener.EndContact(contact);
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	m_fallingListener.PostSolve(contact, impulse);
	m_actorIDListener.PostSolve(contact, impulse);
}