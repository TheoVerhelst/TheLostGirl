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
	//Perform various checks about components
	if(not entity)
		return;
	ArticuledArmsComponent::Handle armsComponent(entity.component<ArticuledArmsComponent>());
	ArcherComponent::Handle archerComponent(entity.component<ArcherComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(not (armsComponent and archerComponent->notchedArrow.valid() and directionComponent))
		return;
	HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
	if(not (holdItemComponent and holdItemComponent->item.valid()))
		return;
	BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
	if(not bowComponent)
		return;

	const float angle{armsComponent->armsJoint->GetJointAngle()};
	const float power{bowComponent->targetTranslation};
	BodyComponent::Handle notchedArrowBodyComponent(archerComponent->notchedArrow.component<BodyComponent>());
	ArrowComponent::Handle notchedArrowArrowComponent(archerComponent->notchedArrow.component<ArrowComponent>());
	if(notchedArrowBodyComponent and notchedArrowArrowComponent)
	{
		b2Body* arrowBody{notchedArrowBodyComponent->body};

		//Destroy all joints (e.g. the bow/arrow joint)
		for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
			arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

		b2Vec2 shootImpulse{power*std::cos(angle), -power*std::sin(angle)};
		if(directionComponent->direction == Direction::Left)
			shootImpulse = {power*std::cos(angle+b2_pi), -power*std::sin(angle+b2_pi)};
		arrowBody->ApplyLinearImpulse(arrowBody->GetMass() * archerComponent->initialSpeed * shootImpulse, arrowBody->GetWorldCenter(), true);
		bowComponent->notchedArrow = entityx::Entity();
		notchedArrowArrowComponent->state = ArrowComponent::Fired;
		notchedArrowArrowComponent->shooter = entity;
	}
}
