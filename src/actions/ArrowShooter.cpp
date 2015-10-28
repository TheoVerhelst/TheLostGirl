#include <entityx/Entity.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/actions/ArrowShooter.h>

void ArrowShooter::operator()(entityx::Entity entity) const
{
	//Perform various checks about components
	if(not entity)
		return;
	ArticuledArmsComponent::Handle armsComponent(entity.component<ArticuledArmsComponent>());
	ArcherComponent::Handle archerComponent(entity.component<ArcherComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(not (armsComponent and armsComponent->arms.valid() and directionComponent))
		return;
	HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
	if(not (holdItemComponent and holdItemComponent->item.valid()))
		return;
	BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
	if(not (bowComponent and bowComponent->notchedArrow.valid()))
		return;

	const float angle{armsComponent->targetAngle};
	const float power{bowComponent->targetTranslation};
	BodyComponent::Handle notchedArrowBodyComponent(bowComponent->notchedArrow.component<BodyComponent>());
	ArrowComponent::Handle notchedArrowArrowComponent(bowComponent->notchedArrow.component<ArrowComponent>());
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
