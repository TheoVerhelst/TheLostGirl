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
		b2Body* body = bodyComponent->bodies["main"];
		float targetVelocity = 0.f;
		if(walkComponent->effectiveMovement ==  Direction::Left)
			targetVelocity = -walkComponent->walkSpeed - body->GetLinearVelocity().x;
		else if(walkComponent->effectiveMovement ==  Direction::Right)
			targetVelocity = walkComponent->walkSpeed - body->GetLinearVelocity().x;
		else if(walkComponent->effectiveMovement ==  Direction::None)
			targetVelocity = -body->GetLinearVelocity().x;
		//Apply an impulse relatively to the mass of the body
		body->ApplyLinearImpulse({targetVelocity*body->GetMass()/4, 0.f}, body->GetWorldCenter(), true);
	}
	//Update the archers
	for(auto entity : entityManager.entities_with_components(bodyComponent, bendComponent))
	{
		float angleTarget{-bendComponent->angle};
		float32 gain = 10.f;
		b2Body* bodyArms = bodyComponent->bodies["arms"];
		b2RevoluteJoint* jointArms = static_cast<b2RevoluteJoint*>(bodyArms->GetJointList()->joint);
		float32 angleError = jointArms->GetJointAngle() - angleTarget;
		jointArms->SetMotorSpeed(-gain * angleError);
		
		b2Body* bodyBow = bodyComponent->bodies["bow"];
		b2RevoluteJoint* jointBow = static_cast<b2RevoluteJoint*>(bodyBow->GetJointList()->joint);
		angleError = jointBow->GetJointAngle() - angleTarget;
		jointBow->SetMotorSpeed(-gain * angleError);
	}
	//Update the sprite transformation according to the one of the b2Body.
	for(auto entity : entityManager.entities_with_components(bodyComponent, spriteComponent))
	{
		std::map<std::string, sf::Sprite>& sprites(spriteComponent->sprites);
		std::map<std::string, sf::Vector2f>& worldPositions(spriteComponent->worldPositions);
		std::map<std::string, b2Body*>& bodies(bodyComponent->bodies);
		for(auto& bodyPair : bodies)
		{
			//If the body exists in sprites and positions maps
			if(sprites.find(bodyPair.first) != sprites.end()
				and worldPositions.find(bodyPair.first) != worldPositions.end())
			{
				b2Vec2 pos = bodyPair.second->GetPosition();
				float32 angle = bodyPair.second->GetAngle();
				worldPositions[bodyPair.first].x = pos.x * m_parameters.pixelScale;
				worldPositions[bodyPair.first].y = pos.y * m_parameters.pixelScale;
				sprites[bodyPair.first].setRotation(angle*180/b2_pi);
			}
		}
	}
}
