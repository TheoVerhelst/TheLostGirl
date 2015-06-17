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
	ArcherComponent::Handle archerComponent(entity.component<ArcherComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(archerComponent and directionComponent)
	{
		//If the notched arrow has a b2Body
		if(archerComponent->notchedArrow.valid())
		{
			BodyComponent::Handle notchedArrowBodyComponent(archerComponent->notchedArrow.component<BodyComponent>());
			ArrowComponent::Handle notchedArrowArrowComponent(archerComponent->notchedArrow.component<ArrowComponent>());
			if(notchedArrowBodyComponent and notchedArrowArrowComponent)
			{
				b2Body* arrowBody{notchedArrowBodyComponent->body};

				//Destroy all joints (e.g. the bow/arrow joint)
				for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
					arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

				b2Vec2 shootImpulse{archerComponent->power*std::cos(archerComponent->angle),
								 -archerComponent->power*std::sin(archerComponent->angle)};
				if(directionComponent->direction == Direction::Left)
					shootImpulse = {archerComponent->power*std::cos(archerComponent->angle+b2_pi),
								 -archerComponent->power*std::sin(archerComponent->angle+b2_pi)};
				arrowBody->ApplyLinearImpulse(arrowBody->GetMass()*archerComponent->initialSpeed*shootImpulse, arrowBody->GetWorldCenter(), true);
				archerComponent->notchedArrow = entityx::Entity();
				notchedArrowArrowComponent->state = ArrowComponent::Fired;
			}
		}
	}
}
