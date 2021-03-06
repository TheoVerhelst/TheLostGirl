#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/actions/ArrowShooter.hpp>

void ArrowShooter::operator()(entityx::Entity entity) const
{
	//Perform various checks about components
	if(not TEST(entity.valid()))
		return;
	ArticuledArmsComponent::Handle armsComponent(entity.component<ArticuledArmsComponent>());
	ArcherComponent::Handle archerComponent(entity.component<ArcherComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(not TEST(armsComponent and armsComponent->arms.valid() and directionComponent))
		return;
	HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
	if(not TEST(holdItemComponent and holdItemComponent->item.valid()))
		return;
	BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
	if(not TEST(bowComponent and bowComponent->notchedArrow.valid()))
		return;

	const float angle{armsComponent->targetAngle};
	const float power{bowComponent->targetTranslation};
	BodyComponent::Handle notchedArrowBodyComponent(bowComponent->notchedArrow.component<BodyComponent>());
	ArrowComponent::Handle notchedArrowArrowComponent(bowComponent->notchedArrow.component<ArrowComponent>());
	if(not TEST(notchedArrowBodyComponent and notchedArrowArrowComponent))
		return;
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
