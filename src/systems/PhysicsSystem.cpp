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
	const float windStrength{m_systemManager.system<TimeSystem>()->getWindStrength()};
	BodyComponent::Handle bodyComponent;
	for(auto entity : entityManager.entities_with_components(bodyComponent))
	{
		b2Body* body{nullptr};
		auto bodyIt(bodyComponent->bodies.find("main"));
		if(bodyIt != bodyComponent->bodies.end())
			body = bodyIt->second;
		//Update the walkers
		const WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
		if(body and walkComponent)
		{
			float targetVelocity{0.f};
			const float walkVelocity{walkComponent->walkSpeed/m_pixelByMeter};
			const Direction walkDirection{walkComponent->effectiveMovement};
			if(walkDirection ==  Direction::Left)
				targetVelocity = -walkVelocity - body->GetLinearVelocity().x;
			else if(walkDirection ==  Direction::Right)
				targetVelocity = walkVelocity - body->GetLinearVelocity().x;
			else if(walkDirection ==  Direction::None)
				targetVelocity = -body->GetLinearVelocity().x;
			//Apply an impulse relatively to the mass of the body
			body->ApplyLinearImpulse({targetVelocity*body->GetMass(), 0.f}, body->GetWorldCenter(), true);
		}

		//Update the jumpers
		JumpComponent::Handle jumpComponent(entity.component<JumpComponent>());
		if(body and jumpComponent and jumpComponent->mustJump)
		{
			const float targetVelocity{-jumpComponent->jumpStrength/m_pixelByMeter};
			body->ApplyLinearImpulse({0.f, targetVelocity*body->GetMass()}, body->GetWorldCenter(), true);
			jumpComponent->mustJump = false;
		}

		//Update the archers
		BowComponent::Handle bowComponent(entity.component<BowComponent>());
		if(bowComponent)
		{
			const float angleTarget{-bowComponent->angle};
			const float32 gain{20.f};
			float32 differenceAngle;
			auto armsBodyIt(bodyComponent->bodies.find("arms"));
			if(armsBodyIt != bodyComponent->bodies.end())
			{
				//Iterate over all joints
				for(b2JointEdge* jointEdge{armsBodyIt->second->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
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
			auto bowBodyIt(bodyComponent->bodies.find("bow"));
			if(bowBodyIt != bodyComponent->bodies.end())
			{
				//Iterate over all joints
				for(b2JointEdge* jointEdge{bowBodyIt->second->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
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

			const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
			if(directionComponent)
			{
				entityx::Entity notchedArrow{bowComponent->notchedArrow};
				//If the notched arrow has a b2Body,
				//Set the translation of the joint between bow and arrow
				if(notchedArrow.valid())
				{
					BodyComponent::Handle arrowBodyComponent(notchedArrow.component<BodyComponent>());
					if(arrowBodyComponent)
					{
						auto arrowBodyIt(arrowBodyComponent->bodies.find("main"));
						if(arrowBodyIt != arrowBodyComponent->bodies.end())
						{
							float translationTarget{bowComponent->power};
							if(directionComponent->direction == Direction::Left)
								translationTarget = 1 - translationTarget;
							float32 differenceTranslation;
							//Iterate over all joints
							for(b2JointEdge* jointEdge{arrowBodyIt->second->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
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
			}
		}

		//Update the arrows
		const ArrowComponent::Handle arrowComponent(entity.component<ArrowComponent>());
		if(body and arrowComponent)
		{
			if(arrowComponent->state == ArrowComponent::Fired)
			{
				body->ApplyForce({windStrength*body->GetMass(), 0}, body->GetWorldCenter(), true);
				//Apply a drag force to point to the direction of the trajectory
				const b2Vec2 pointingDirection{body->GetWorldVector(b2Vec2(1, 0))};//Get the global direction of the arrow
				b2Vec2 flightDirection{body->GetLinearVelocity()};//Get the effective flight direction of the arrow
				const float flightSpeed{flightDirection.Normalize()};//Normalizes (v in ([0, 1], [0, 1])) and returns length
				const float scalarProduct{b2Dot(flightDirection, pointingDirection)};
				const float dragConstant{arrowComponent->friction};
				//The first commented line apply a lower force on the arrow when the arrow is diriged to the opposite side of its trajectory,
				//So if the arrow is fired vertically, it will be decelered less than with the second line.
		//		float dragForceMagnitude{(1 - fabs(scalarProduct)) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				const float dragForceMagnitude{(1.f - scalarProduct) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				//Convert the local friction point to Box2D global coordinates
				const b2Vec2 localFrictionPoint{sftob2(arrowComponent->localFrictionPoint/m_pixelByMeter)};
				const b2Vec2 arrowTailPosition{body->GetWorldPoint(localFrictionPoint)};
				body->ApplyForce(float32(dragForceMagnitude)*(-flightDirection), arrowTailPosition, true);
			}

		}

		//Update the transformations according to the one of the b2Body.
		TransformComponent::Handle transformComponent(entity.component<TransformComponent>());
		if(transformComponent)
		{
			for(auto& bodyPair : bodyComponent->bodies)
			{
				auto transformIt(transformComponent->transforms.find(bodyPair.first));
				//If the name of the body exists in the transforms maps
				if(transformIt != transformComponent->transforms.end())
				{
					b2Vec2 pos{bodyPair.second->GetPosition()};
					float32 angle{bodyPair.second->GetAngle()};
					transformIt->second.x = pos.x * m_pixelByMeter;
					transformIt->second.y = pos.y * m_pixelByMeter;
					transformIt->second.angle = angle*180/b2_pi;
				}
			}
		}
	}
}
