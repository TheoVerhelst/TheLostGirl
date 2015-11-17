#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/ContactListener.h>


void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	const bool collision{collide(contact, oldManifold)};
	contact->SetEnabled(collision);
	if(collision)
	{
		m_fallingListener.PreSolve(contact, oldManifold);
		m_arrowHitListener.PreSolve(contact, oldManifold);
	}
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

	//The contact does not occurs if an entity is invalid
	if(not entityA or not entityB)
		return false;

	//The contact does not occurs if both entities are the same one
	if(entityA == entityB)
		return false;

	//The contact does not occurs if both entities are actors
	if(entityA.has_component<ActorComponent>() and entityB.has_component<ActorComponent>())
		return false;

	const TransformComponent::Handle entityATransformComponent(entityA.component<TransformComponent>());
	const TransformComponent::Handle entityBTransformComponent(entityB.component<TransformComponent>());
	//The contact does not occurs if the entities are not in the same plan
	if(entityATransformComponent and entityBTransformComponent
		and lround(entityATransformComponent->transform.z) != lround(entityBTransformComponent->transform.z))
			return false;

	return checkCollide(entityA, entityB) and checkCollide(entityB, entityA);
}

bool ContactListener::checkCollide(entityx::Entity entityA, entityx::Entity entityB) const
{
	const ArrowComponent::Handle entityAArrowComponent{entityA.component<ArrowComponent>()};
	const ActorComponent::Handle entityAActorComponent{entityA.component<ActorComponent>()};
	const ArrowComponent::Handle entityBArrowComponent{entityB.component<ArrowComponent>()};
	const DeathComponent::Handle entityBDeathComponent{entityB.component<DeathComponent>()};
	const QuiverComponent::Handle entityBQuiverComponent{entityB.component<QuiverComponent>()};
	const BowComponent::Handle entityBBowComponent{entityB.component<BowComponent>()};
	const HoldItemComponent::Handle entityBHoldItemComponent{entityB.component<HoldItemComponent>()};
	const ArticuledArmsComponent::Handle entityBArticuledArmsComponent{entityB.component<ArticuledArmsComponent>()};

	//The contact does not occurs if both entities are arrows
	if(entityAArrowComponent and entityBArrowComponent)
		return false;

	//The contact does not occurs if the entity A is an arrow wich is not fired
	if(entityAArrowComponent and entityAArrowComponent->state != ArrowComponent::Fired)
		return false;

	//The contact does not occurs if the entity A was fired by B
	if(entityAArrowComponent and entityAArrowComponent->shooter == entityB)
		return false;

	//The contact does occurs if the entity A is in the quiver B
	if(entityBQuiverComponent and entityBQuiverComponent->arrows.find(entityA) != entityBQuiverComponent->arrows.end())
		return false;

	//The contact does not occurs if the entity B hold the item A
	if(entityBHoldItemComponent and entityBHoldItemComponent->item == entityA)
		return false;

	//The contact does not occurs if the arms A are on the body B
	if(entityBArticuledArmsComponent and entityBArticuledArmsComponent->arms == entityA)
		return false;

	//The contact does not occurs if the entity A is a corpse and the entity B is an arrow
	if(entityBDeathComponent and entityBDeathComponent->dead and entityAArrowComponent)
		return false;

	//The contact does not occurds if the entity A is an actor and the entity B is arms, a quiver or a bow
	if(entityAActorComponent and (entityBHoldItemComponent or entityBQuiverComponent or entityBBowComponent))
		return false;

	//If none of the previous case is verified, then the collision occurs
	return true;
}



void ContactListener::BeginContact(b2Contact* contact)
{
	contact->SetEnabled(collide(contact, nullptr));
	if(contact->IsEnabled())
	{
		m_fallingListener.BeginContact(contact);
		m_arrowHitListener.BeginContact(contact);
	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	contact->SetEnabled(collide(contact, nullptr));
	if(contact->IsEnabled())
	{
		m_fallingListener.EndContact(contact);
		m_arrowHitListener.EndContact(contact);
	}
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	contact->SetEnabled(collide(contact, nullptr));
	if(contact->IsEnabled())
	{
		m_fallingListener.PostSolve(contact, impulse);
		m_arrowHitListener.PostSolve(contact, impulse);
	}
}
