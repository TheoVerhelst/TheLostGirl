#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/ContactListener.h>

ContactListener::ContactListener(StateStack::Context context):
	m_context(context),
	m_fallingListener(m_context),
	m_arrowHitListener(m_context)
{
}


void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	contact->SetEnabled(collide(contact, oldManifold));
	m_fallingListener.PreSolve(contact, oldManifold);
	m_arrowHitListener.PreSolve(contact, oldManifold);
}

bool ContactListener::collide(b2Contact* contact, const b2Manifold*)
{
	//Contact filtering
	//The filtering must be done here because the b2ContactFilter class is called
	//only one time per contact, and that allow bodies overlapping to stay overlapped.
	//If the filtering is done here, the filter calculation is done every step of the contact.
	b2Fixture* fixtureA{contact->GetFixtureA()};
	b2Body* bodyA{fixtureA->GetBody()};
	entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};
	b2Fixture* fixtureB{contact->GetFixtureB()};
	b2Body* bodyB{fixtureB->GetBody()};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};

	//Wrap all checks into this function allow to stop next checks when any of the checks is verified.

	//The contact do not occurs if both entities are the same one
	if(entityA == entityB)
		return false;

	//The contact do not occurs if both entities are actors
	if(entityA.has_component<CategoryComponent>()
		and entityA.component<CategoryComponent>()->category & Category::Actor
		and entityB.has_component<CategoryComponent>()
		and entityB.component<CategoryComponent>()->category & Category::Actor)
		return false;

	//If the two entities are not in the same plan
	if(entityA.has_component<TransformComponent>() and entityB.has_component<TransformComponent>())
	{
		BodyComponent::Handle bodiesA{entityA.component<BodyComponent>()};
		TransformComponent::Handle trsfA{entityA.component<TransformComponent>()};
		auto it = std::find_if(bodiesA->bodies.begin(), bodiesA->bodies.end(), [&](const std::pair<std::string, b2Body*>& pair){return pair.second == bodyA;});
		std::string bodyNameA{it->first};
		long int zA{lround(trsfA->transforms[bodyNameA].z)};//Nearest rounding of the plan of the body/sprite A

		BodyComponent::Handle bodiesB{entityB.component<BodyComponent>()};
		TransformComponent::Handle trsfB{entityB.component<TransformComponent>()};
		it = std::find_if(bodiesB->bodies.begin(), bodiesB->bodies.end(), [&](const std::pair<std::string, b2Body*>& pair){return pair.second == bodyB;});
		std::string bodyNameB{it->first};
		long int zB{lround(trsfB->transforms[bodyNameB].z)};//Nearest rounding of the plan of the body/sprite B

		//The contact do not occurs if the entities are not in the same plan
		if(zA != zB)
			return false;
	}

	//The contact do not occurs if both entities are arrows
	if(entityA.has_component<ArrowComponent>() and entityB.has_component<ArrowComponent>())
		return false;

	//The contact do not occurs if the entity A is an arrow wich is not fired
	if(entityA.has_component<ArrowComponent>() and entityA.component<ArrowComponent>()->state != ArrowComponent::Fired)
			return false;

	//The contact do not occurs if the entity B is an arrow wich is not fired
	if(entityB.has_component<ArrowComponent>() and entityB.component<ArrowComponent>()->state != ArrowComponent::Fired)
		return false;

	//If none of the previous case is verified, then the collision occurs
	return true;
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
