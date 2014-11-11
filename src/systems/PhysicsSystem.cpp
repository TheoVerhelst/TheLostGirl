#include <cmath>
#include <iostream>

#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/systems/PhysicsSystem.h>

void PhysicsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	int32 velocityIterations = 8;
	int32 positionIterations = 8;
	m_world.Step(dt, velocityIterations, positionIterations);
	BodyComponent::Handle bodyComponent;
	SpriteComponent::Handle spriteComponent;
	WalkComponent::Handle walkComponent;
	BendComponent::Handle bendComponent;
	//Update the walkers
	for(auto entity : entityManager.entities_with_components(bodyComponent, walkComponent))
	{
		b2Body* body = bodyComponent->body;
		float velocity = 0.f;
		if(walkComponent->effectiveMovement ==  Direction::Left)
			velocity = -walkComponent->walkSpeed;
		else if(walkComponent->effectiveMovement ==  Direction::Right)
			velocity = walkComponent->walkSpeed;
		body->SetLinearVelocity({velocity, body->GetLinearVelocity().y});
	}
	//Update the archers
	for(auto entity : entityManager.entities_with_components(bodyComponent, bendComponent))
	{
		b2Body* body = bodyComponent->body;
		b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(body->GetJointList()->joint);
		float angleTarget{-bendComponent->angle};
		float32 angleError = joint->GetJointAngle() - angleTarget;
		float32 gain = 10.f;
		joint->SetMotorSpeed(-gain * angleError);
	}
	//Update the sprite transformation according to the one of the b2Body.
	for(auto entity : entityManager.entities_with_components(bodyComponent, spriteComponent))
	{
		b2Vec2 pos = bodyComponent->body->GetPosition();
		float32 angle = bodyComponent->body->GetAngle();
		spriteComponent->worldPosition.x = pos.x * m_parameters.pixelScale;
		spriteComponent->worldPosition.y = pos.y * m_parameters.pixelScale;
		spriteComponent->sprite.setRotation(angle*180/b2_pi);
	}
}