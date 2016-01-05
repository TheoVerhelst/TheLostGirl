#include <cmath>
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/Category.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/systems/TimeSystem.hpp>
#include <TheLostGirl/systems/PhysicsSystem.hpp>

void PhysicsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	int32 velocityIterations{8};
	int32 positionIterations{8};
	Context::world->Step(float32(dt), velocityIterations, positionIterations);
	//Get the force of the wind
	const float windStrength{Context::systemManager->system<TimeSystem>()->getWindStrength()};
	BodyComponent::Handle bodyComponent;
	for(auto entity : entityManager.entities_with_components(bodyComponent))
	{
		b2Body* body{bodyComponent->body};
		//Update the walkers
		const WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
		if(walkComponent)
		{
			float targetVelocity{0.f};
			const float walkVelocity{walkComponent->walkSpeed/Context::parameters->pixelByMeter};
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
		if(jumpComponent and jumpComponent->mustJump)
		{
			const float targetVelocity{-jumpComponent->jumpStrength/Context::parameters->pixelByMeter};
			body->ApplyLinearImpulse({0.f, targetVelocity*body->GetMass()}, body->GetWorldCenter(), true);
			jumpComponent->mustJump = false;
		}

		const float32 gain{5.f};
		//Update the rotating arms
		ArticuledArmsComponent::Handle armsComponent(entity.component<ArticuledArmsComponent>());

		if(armsComponent)
			armsComponent->armsJoint->SetMotorSpeed(gain * (-armsComponent->targetAngle - armsComponent->armsJoint->GetJointAngle()));

		//Update the notched translating arrows
		BowComponent::Handle bowComponent(entity.component<BowComponent>());
		if(bowComponent)
		{
			b2PrismaticJoint* joint{bowComponent->notchedArrowJoint};
			float targetTranslation{bowComponent->targetTranslation};
			//The final target is equal to the joint's lower limit when the targetTranslation is equal to 1.
			joint->SetMotorSpeed(gain * ((targetTranslation * joint->GetLowerLimit()) - joint->GetJointTranslation()));
		}

		//Update the arrows
		const ArrowComponent::Handle arrowComponent(entity.component<ArrowComponent>());
		if(arrowComponent)
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
//				float dragForceMagnitude{(1.f - std::abs(scalarProduct)) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				const float dragForceMagnitude{(1.f - scalarProduct) * flightSpeed * flightSpeed * dragConstant * body->GetMass()};
				//Convert the local friction point to Box2D global coordinates
				const b2Vec2 localFrictionPoint{sftob2(arrowComponent->localFrictionPoint/Context::parameters->pixelByMeter)};
				const b2Vec2 arrowTailPosition{body->GetWorldPoint(localFrictionPoint)};
				body->ApplyForce(static_cast<float32>(dragForceMagnitude) * (-flightDirection), arrowTailPosition, true);
			}

		}

		//Update the transformations according to the one of the b2Body.
		TransformComponent::Handle transformComponent(entity.component<TransformComponent>());
		if(transformComponent)
		{
			const b2Vec2 pos{body->GetPosition()};
			transformComponent->transform.x = pos.x * Context::parameters->pixelByMeter;
			transformComponent->transform.y = pos.y * Context::parameters->pixelByMeter;
			transformComponent->transform.angle = body->GetAngle()*180/b2_pi;
		}
	}
}
