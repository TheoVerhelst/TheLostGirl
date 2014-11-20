#include <iostream>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/ContactFilter.h>

bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	b2Body* bodyA = fixtureA->GetBody();
	entityx::Entity* entityA = static_cast<entityx::Entity*>(bodyA->GetUserData());
	b2Body* bodyB = fixtureB->GetBody();
	entityx::Entity* entityB = static_cast<entityx::Entity*>(bodyB->GetUserData());
	bool ret;
	if(entityA == entityB)
		ret =  false;
	else if(entityA->has_component<TransformComponent>() and entityB->has_component<TransformComponent>())
	{
		BodyComponent::Handle bodiesA = entityA->component<BodyComponent>();
		TransformComponent::Handle trsfA = entityA->component<TransformComponent>();
		auto it = bodiesA->bodies.begin();
		while(it != bodiesA->bodies.end() and it->second != bodyA)
			++it;
		if(it == bodiesA->bodies.end())
			throw std::runtime_error("An entity is referenced by the user data of a body, but the entity has not registred this body in BodyComponent");
		std::string bodyNameA{it->first};
		long int zA;//Plan of the body/sprite A
		if(trsfA->transforms.find(bodyNameA) != trsfA->transforms.end())
			zA = lround(trsfA->transforms[bodyNameA].z);//Nearest rounding
		
		BodyComponent::Handle bodiesB = entityB->component<BodyComponent>();
		TransformComponent::Handle trsfB = entityB->component<TransformComponent>();
		it = bodiesB->bodies.begin();
		while(it != bodiesB->bodies.end() and it->second != bodyB)
			++it;
		if(it == bodiesB->bodies.end())
			throw std::runtime_error("An entity is referenced by the user data of a body, but the entity has not registred this body in BodyComponent");
		std::string bodyNameB{it->first};
		long int zB;//Plan of the body/sprite B
		if(trsfB->transforms.find(bodyNameB) != trsfB->transforms.end())
			zB = lround(trsfB->transforms[bodyNameB].z);//Nearest rounding
		
		return zA == zB;//Collide only if in the same plan
	}
	else
		ret = true;
	return ret;
}
