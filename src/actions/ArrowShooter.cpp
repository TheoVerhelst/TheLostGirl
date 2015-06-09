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
	auto bowComponent(entity.component<BowComponent>());
	const auto directionComponent(entity.component<DirectionComponent>());
	if(bowComponent and directionComponent)
	{
		//If the notched arrow has a b2Body
		if(bowComponent->notchedArrow.valid())
		{
			auto notchedArrowBodyComponent(bowComponent->notchedArrow.component<BodyComponent>());
			auto notchedArrowArrowComponent(bowComponent->notchedArrow.component<ArrowComponent>());
			if(notchedArrowBodyComponent and notchedArrowArrowComponent)
			{
				auto arrowBodyIt(notchedArrowBodyComponent->bodies.find("main"));
				if(arrowBodyIt != notchedArrowBodyComponent->bodies.end())
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
					notchedArrowArrowComponent->state = ArrowComponent::Fired;
				}
			}
		}
	}
}
