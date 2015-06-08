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
		auto arrowBodyIt(notchedArrow.component<BodyComponent>()->bodies.find("main"));
		//If the notched arrow has a b2Body
		if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>() and notchedArrow.has_component<ArrowComponent>()
			and arrowBodyIt != notchedArrow.component<BodyComponent>()->bodies.end())
		{
			b2Body* arrowBody{arrowBodyIt->second};

			//Destroy all joints (e.g. the bow/arrow joint)
			for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

			b2Vec2 shootImpulse{bowComponent->power*std::cos(bowComponent->angle),
							 -bowComponent->power*std::sin(bowComponent->angle)};
			if(directionComponent->direction == Direction::Left)
				shootImpulse = {bowComponent->power*std::cos(bowComponent->angle+b2_pi),
							 -bowComponent->power*std::sin(bowComponent->angle+b2_pi)};
			arrowBody->ApplyLinearImpulse(arrowBody->GetMass()*bowComponent->initialSpeed*shootImpulse, arrowBody->GetWorldCenter(), true);
			bowComponent->notchedArrow = entityx::Entity();
			notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Fired;
		}
	}
}
