#include <cmath>
#include <iostream>

#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/JointRoles.h>

#include <TheLostGirl/systems/PhysicsSystem.h>

void PhysicsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	int32 velocityIterations = 8;
	int32 positionIterations = 8;
	m_world.Step(dt, velocityIterations, positionIterations);
	BodyComponent::Handle bodyComponent;
	TransformComponent::Handle transformComponent;
	WalkComponent::Handle walkComponent;
	BendComponent::Handle bendComponent;
	//Update the walkers
	for(auto entity : entityManager.entities_with_components(bodyComponent, walkComponent))
	{
		//If the walker has not a main body, the program will crash
		assert(bodyComponent->bodies.find("main") != bodyComponent->bodies.end());
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
		float32 angleError;
		//If the archer has not a arms body, the program will crash
		assert(bodyComponent->bodies.find("arms") != bodyComponent->bodies.end());
		b2Body* bodyArms = bodyComponent->bodies["arms"];
		//Iterate over all joints
		for(b2JointEdge* jointEdge{bodyArms->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
		{
			//If this is a bending joint
			if(jointHasRole(jointEdge->joint, JointRole::BendingAngle))
			{
				b2RevoluteJoint* jointArms = static_cast<b2RevoluteJoint*>(jointEdge->joint);
				angleError = jointArms->GetJointAngle() - angleTarget;
				jointArms->SetMotorSpeed(-gain * angleError);
			}
		}
		
		//If the archer has not a bow body, the program will crash
		assert(bodyComponent->bodies.find("bow") != bodyComponent->bodies.end());
		b2Body* bodyBow = bodyComponent->bodies["bow"];
		//Iterate over all joints
		for(b2JointEdge* jointEdge{bodyBow->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
		{
			//If this is a bending joint
			if(jointHasRole(jointEdge->joint, JointRole::BendingAngle))
			{
				b2RevoluteJoint* jointBow = static_cast<b2RevoluteJoint*>(jointEdge->joint);
				angleError = jointBow->GetJointAngle() - angleTarget;
				jointBow->SetMotorSpeed(-gain * angleError);
			}
		}
	}
	//Update the transformations according to the one of the b2Body.
	for(auto entity : entityManager.entities_with_components(bodyComponent, transformComponent))
	{
		std::map<std::string, Transform>& transforms(transformComponent->transforms);
		std::map<std::string, b2Body*>& bodies(bodyComponent->bodies);
		for(auto& bodyPair : bodies)
		{
			//If the name of the body exists in the transforms maps
			if(transforms.find(bodyPair.first) != transforms.end())
			{
				b2Vec2 pos = bodyPair.second->GetPosition();
				float32 angle = bodyPair.second->GetAngle();
				transforms[bodyPair.first].x = pos.x * m_parameters.scaledPixelByMeter;
				transforms[bodyPair.first].y = pos.y * m_parameters.scaledPixelByMeter;
				transforms[bodyPair.first].angle = angle*180/b2_pi;
			}
		}
	}
}
