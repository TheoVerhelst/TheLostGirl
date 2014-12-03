#include <TheLostGirl/ContactListener.h>

ContactListener::ContactListener(State::Context context):
	m_context(context),
	m_fallingListener(m_context),
	m_arrowHitListener(m_context)
{}


void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	m_fallingListener.PreSolve(contact, oldManifold);
	m_arrowHitListener.PreSolve(contact, oldManifold);
}

void ContactListener::BeginContact(b2Contact* contact)
{
	m_fallingListener.BeginContact(contact);
	m_arrowHitListener.BeginContact(contact);
}

void ContactListener::EndContact(b2Contact* contact)
{
	m_fallingListener.EndContact(contact);
	m_arrowHitListener.EndContact(contact);
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	m_fallingListener.PostSolve(contact, impulse);
	m_arrowHitListener.PostSolve(contact, impulse);
}
