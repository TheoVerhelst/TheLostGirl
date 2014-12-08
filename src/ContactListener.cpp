#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/ContactListener.h>

ContactListener::ContactListener(State::Context context):
	m_context(context),
	m_fallingListener(m_context),
	m_arrowHitListener(m_context)
{}


void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
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
	bool enableContact{true};
	//The &&=  assignement operator allow to specify that all operands right to the assignement must be true
	//in order to enable the collision. So by default collision is enabled,
	//and if at least one condition is false, the collision will not occurs.
	
	//If both entities are the same one
	enableContact = enableContact && entityA != entityB;
	
	//If the two entities are not in the same plan
	if(entityA.has_component<TransformComponent>() and entityB.has_component<TransformComponent>())
	{
		BodyComponent::Handle bodiesA{entityA.component<BodyComponent>()};
		TransformComponent::Handle trsfA{entityA.component<TransformComponent>()};
		auto it = bodiesA->bodies.begin();
		while(it != bodiesA->bodies.end() and it->second != bodyA)
			++it;
		if(it == bodiesA->bodies.end())
			throw std::runtime_error("An entity is referenced by the user data of a body, but the entity has not registred this body in BodyComponent");
		std::string bodyNameA{it->first};
		long int zA{lround(trsfA->transforms[bodyNameA].z)};//Nearest rounding of the plan of the body/sprite A
		
		BodyComponent::Handle bodiesB{entityB.component<BodyComponent>()};
		TransformComponent::Handle trsfB{entityB.component<TransformComponent>()};
		it = bodiesB->bodies.begin();
		while(it != bodiesB->bodies.end() and it->second != bodyB)
			++it;
		if(it == bodiesB->bodies.end())
			throw std::runtime_error("An entity is referenced by the user data of a body, but the entity has not registred this body in BodyComponent");
		std::string bodyNameB{it->first};
		long int zB{lround(trsfB->transforms[bodyNameB].z)};//Nearest rounding of the plan of the body/sprite B
		
		enableContact = enableContact && (zA == zB);//Collide only if in the same plan
	}
	
	//If an entity is an arrow of the other one
	if(entityA.has_component<BowComponent>() or entityB.has_component<BowComponent>())
	{
		if(entityA.has_component<BowComponent>())
		{
			//Swap the pointers
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		const std::vector<entityx::Entity>& arrows(entityB.component<BowComponent>()->arrows);
		//Right operand is false if the entity A is in the quiver of the entity B
		enableContact = enableContact && std::find(arrows.begin(), arrows.end(), entityA) == arrows.end();
		enableContact = enableContact && entityA != entityB.component<BowComponent>()->notchedArrow;
	}
	
	//If both entities are arrows
	//If the left operand is false, then the collison do not occurs
	enableContact = enableContact && (not (entityA.has_component<ArrowComponent>() and entityB.has_component<ArrowComponent>()));
	
	//If the entity A is an arrow, collide only if it is not sticked
	if(entityA.has_component<ArrowComponent>())
		enableContact = enableContact && not entityA.component<ArrowComponent>()->sticked;
		
	//If the entity B is an arrow, collide only if it is not sticked
	if(entityB.has_component<ArrowComponent>())
		enableContact = enableContact && not entityB.component<ArrowComponent>()->sticked;
	
	//If an entity is a sticked arrow and the other one an actor
	if((entityA.has_component<CategoryComponent>() and entityB.has_component<ArrowComponent>())
		or (entityB.has_component<CategoryComponent>() and entityA.has_component<ArrowComponent>()))
	{
		//If entity A is maybe an actor and entity B an arrow
		if(entityA.has_component<CategoryComponent>() and entityB.has_component<ArrowComponent>())
		{
			//Swap the pointers
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		if(entityA.component<ArrowComponent>()->sticked)
			//Now entity A is a sticked arrow and entity B is maybe an actor
			//Right operand is false if the entity B is an actor
			enableContact = enableContact && (not (entityB.component<CategoryComponent>()->category & Category::Actor));
	}
	contact->SetEnabled(enableContact);
	
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
