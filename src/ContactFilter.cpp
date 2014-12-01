#include <iostream>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/ContactFilter.h>

bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	b2Body* bodyA{fixtureA->GetBody()};
	entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};
	b2Body* bodyB{fixtureB->GetBody()};
	entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};
	bool ret{true};
	//The &&=  assignement operator allow to specify that all operands right to the assignement must be true
	//in order to enable the collision. So by default collision is enabled,
	//and if at least one condition is false, the collision will not occurs.
	ret = ret && entityA != entityB;
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
		
		ret = ret && zA == zB;//Collide only if in the same plan
	}
	if(entityA.has_component<QuiverComponent>() or entityB.has_component<QuiverComponent>())
	{
		if(entityA.has_component<QuiverComponent>())
		{
			//Swap the pointers
			std::swap(bodyA, bodyB);
			std::swap(entityA, entityB);
		}
		const std::vector<entityx::Entity>& arrows(entityB.component<QuiverComponent>()->arrows);
		//Right operand is false the entity A is in the quiver of the entity B
		ret = ret && std::find(arrows.begin(), arrows.end(), entityA) == arrows.end();
		ret = ret && entityA != entityB.component<QuiverComponent>()->notchedArrow;
	}
	return ret;
}
