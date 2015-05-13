#include <entityx/Entity.h>
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/actions/ArrowShooter.h>

ArrowShooter::ArrowShooter()
{
}

ArrowShooter::~ArrowShooter()
{
}

void ArrowShooter::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	if(entity.has_component<BowComponent>()
		and entity.has_component<DirectionComponent>())
	{
		BowComponent::Handle bowComponent{entity.component<BowComponent>()};
		entityx::Entity notchedArrow{bowComponent->notchedArrow};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//If the notched arrow has a b2Body
		if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>() and notchedArrow.has_component<ArrowComponent>()
			and notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end())
		{
			b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies.at("main")};

			//Destroy all joints (e.g. the bow/arrow joint)
			for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

			float shootForceX{bowComponent->power*static_cast<float>(cos(bowComponent->angle))};
			float shootForceY{-bowComponent->power*static_cast<float>(sin(bowComponent->angle))};
			if(directionComponent->direction == Direction::Left)
			{
				shootForceX = bowComponent->power*static_cast<float>(cos(bowComponent->angle+b2_pi));
				shootForceY = -bowComponent->power*static_cast<float>(sin(bowComponent->angle+b2_pi));
			}
			b2Vec2 shootForce{static_cast<float32>(shootForceX), static_cast<float32>(shootForceY)};
			arrowBody->ApplyLinearImpulse((arrowBody->GetMass()/20.f)*shootForce, arrowBody->GetWorldCenter(), true);
			bowComponent->notchedArrow = entityx::Entity();
			notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Fired;
		}
	}
}
