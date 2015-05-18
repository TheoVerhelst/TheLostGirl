#include <cmath>
#include <iostream>

#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/JointRoles.h>
#include <TheLostGirl/systems/TimeSystem.h>

#include <TheLostGirl/systems/PhysicsSystem.h>

PhysicsSystem::PhysicsSystem(StateStack::Context context):
	m_world(context.world),
	m_pixelByMeter(context.parameters.pixelByMeter),
	m_systemManager(context.systemManager)
{}

void PhysicsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	int32 velocityIterations{8};
	int32 positionIterations{8};
	m_world.Step(float32(dt), velocityIterations, positionIterations);
	//Get the force of the wind
	float windStrength{m_systemManager.system<TimeSystem>()->getWindStrength()};
	BodyComponent::Handle bodyComponent;
	for(auto entity : entityManager.entities_with_components(bodyComponent))
	{
		b2Body* body{bodyComponent->bodies.find("main") != bodyComponent->bodies.end() ? bodyComponent->bodies.at("main"): nullptr};
		//Update the walkers
		if(body and entity.has_component<WalkComponent>())
		{
			WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
			float targetVelocity{0.f};
			float walkVelocity{walkComponent->walkSpeed/m_pixelByMeter};
			if(walkComponent->effectiveMovement ==  Direction::Left)
				targetVelocity = -walkVelocity - body->GetLinearVelocity().x;
			else if(walkComponent->effectiveMovement ==  Direction::Right)
				targetVelocity = walkVelocity - body->GetLinearVelocity().x;
			else if(walkComponent->effectiveMovement ==  Direction::None)
				targetVelocity = -body->GetLinearVelocity().x;
			//Apply an impulse relatively to the mass of the body
			body->ApplyLinearImpulse({targetVelocity*body->GetMass(), 0.f}, body->GetWorldCenter(), true);
		}

		//Update the jumpers
		if(body and entity.has_component<JumpComponent>() and entity.component<JumpComponent>()->mustJump)
		{
			float targetVelocity{-entity.component<JumpComponent>()->jumpStrength/m_pixelByMeter};
			body->ApplyLinearImpulse({0.f, targetVelocity*body->GetMass()}, body->GetWorldCenter(), true);
			entity.component<JumpComponent>()->mustJump = false;
		}

		//Update the archers
		if(entity.has_component<BowComponent>())
		{
			BowComponent::Handle bowComponent(entity.component<BowComponent>());
			float angleTarget{-bowComponent->angle};
			float32 gain{20.f};
			float32 differenceAngle;
			if(bodyComponent->bodies.find("arms") != bodyComponent->bodies.end())
			{
				b2Body* bodyArms{bodyComponent->bodies["arms"]};
				//Iterate over all joints
				for(b2JointEdge* jointEdge{bodyArms->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				{
					//If this is a bending joint
					if(jointHasRole(jointEdge->joint, JointRole::BendingAngle))
					{
						b2RevoluteJoint* jointArms{static_cast<b2RevoluteJoint*>(jointEdge->joint)};
						differenceAngle = angleTarget - jointArms->GetJointAngle();
						jointArms->SetMotorSpeed(gain * differenceAngle);
					}
				}
			}

			if(bodyComponent->bodies.find("bow") != bodyComponent->bodies.end())
			{
				b2Body* bodyBow{bodyComponent->bodies["bow"]};
				//Iterate over all joints
				for(b2JointEdge* jointEdge{bodyBow->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				{
					//If this is a bending joint
					if(jointHasRole(jointEdge->joint, JointRole::BendingAngle))
					{
						b2RevoluteJoint* jointBow{static_cast<b2RevoluteJoint*>(jointEdge->joint)};
						differenceAngle = angleTarget - jointBow->GetJointAngle();
						jointBow->SetMotorSpeed(gain * differenceAngle);
					}
				}
			}
			if(entity.has_component<DirectionComponent>())
			{
				Direction direction{entity.component<DirectionComponent>()->direction};
				entityx::Entity notchedArrow{bowComponent->notchedArrow};
				//If the notched arrow has a b2Body,
				//Set the translation of the joint between bow and arrow
				if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>()
					and notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end())
				{
					b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
					float translationTarget{bowComponent->power/bowComponent->maxPower};//Power of the bending, in range [0, 1]
					if(direction == Direction::Left)
						translationTarget = 1 - translationTarget;
					float32 differenceTranslation;
					//Iterate over all joints
					for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
					{
						//If this is a bending translation joint
						if(jointHasRole(jointEdge->joint, JointRole::BendingPower))
						{
							b2PrismaticJoint* joint{static_cast<b2PrismaticJoint*>(jointEdge->joint)};
							//The final target is equal to the joint's lower limit when the translationTarget is equal to 1.
							differenceTranslation = translationTarget*joint->GetLowerLimit() - joint->GetJointTranslation();
							joint->SetMotorSpeed(gain * differenceTranslation);
						}
					}
				}
			}
		}

		//Update the arrows
		if(body and entity.has_component<ArrowComponent>())
		{
			ArrowComponent::Handle arrowComponent(entity.component<ArrowComponent>());
			if(arrowComponent->state == ArrowComponent::Fired)
			{
				body->ApplyForce({windStrength*body->GetMass(), 0}, body->GetWorldCenter(), true);
				//Apply a drag force to point to the direction of the trajectory
				b2Vec2 pointingDirection{body->GetWorldVector(b2Vec2(1, 0))};//Get the global direction of the arrow
				b2Vec2 flightDirection{body->GetLinearVelocity()};//Get the effective flight direction of the arrow
				float flightSpeed{flightDirection.Normalize()};//Normalizes (v in ([0, 1], [0, 1])) and returns length
				float scalarProduct{b2Dot(flightDirection, pointingDirection)};
				float dragConstant{arrowComponent->friction};
				//The first commented line apply a lower force on the arrow when the arrow is diriged to the opposite side of its trajectory,
				//So if the arrow is fired vertically, it will be decelered less than with the second line.
		//		float dragForceMagnitude{(1 - fabs(scalarProduct)) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				float dragForceMagnitude{(1.f - scalarProduct) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				//Convert the local friction point to Box2D global coordinates
				b2Vec2 localFrictionPoint{sftob2(arrowComponent->localFrictionPoint/m_pixelByMeter)};
				b2Vec2 arrowTailPosition{body->GetWorldPoint(localFrictionPoint)};
				body->ApplyForce(float32(dragForceMagnitude)*(-flightDirection), arrowTailPosition, true);
			}

		}

		//Update the transformations according to the one of the b2Body.
		if(entity.has_component<TransformComponent>())
		{
			TransformComponent::Handle transformComponent(entity.component<TransformComponent>());
			auto& transforms(transformComponent->transforms);
			auto& bodies(bodyComponent->bodies);
			for(auto& bodyPair : bodies)
			{
				//If the name of the body exists in the transforms maps
				if(transforms.find(bodyPair.first) != transforms.end())
				{
					b2Vec2 pos{bodyPair.second->GetPosition()};
					float32 angle{bodyPair.second->GetAngle()};
					transforms[bodyPair.first].x = pos.x * m_pixelByMeter;
					transforms[bodyPair.first].y = pos.y * m_pixelByMeter;
					transforms[bodyPair.first].angle = angle*180/b2_pi;
				}
			}
		}
	}
}
