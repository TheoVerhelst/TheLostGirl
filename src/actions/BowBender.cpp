#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/actions/BowBender.hpp>

BowBender::BowBender(float _angle, float _power):
	angle{_angle},
	power{_power}
{
}

void BowBender::operator()(entityx::Entity entity) const
{
	if(not TEST(entity.valid()))
		return;
	ArcherComponent::Handle archerComponent{entity.component<ArcherComponent>()};
	ArticuledArmsComponent::Handle armsComponent{entity.component<ArticuledArmsComponent>()};
	BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
	DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
	if(not TEST(archerComponent and archerComponent->quiver.valid())
			and bodyComponent and directionComponent
			and armsComponent and armsComponent->arms.valid())
		return;
	HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
	if(not TEST(holdItemComponent and holdItemComponent->item.valid()))
		return;
	BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
	QuiverComponent::Handle quiverComponent{archerComponent->quiver.component<QuiverComponent>()};
	BodyComponent::Handle bowBodyComponent{holdItemComponent->item.component<BodyComponent>()};
	if(not TEST(bowComponent and quiverComponent and bowBodyComponent))
		return;
	//Set the bending angle
	if(directionComponent->direction == Direction::Left)
		armsComponent->targetAngle = cap(angle - b2_pi, -b2_pi, b2_pi/2.f);
	else if(directionComponent->direction == Direction::Right)
		armsComponent->targetAngle = cap(angle, -b2_pi/2.f, b2_pi);
	//Set the bending power, limited to 500px of drag and drop
	bowComponent->targetTranslation = cap(power, 0.f, 500.f)/500.f;

	//If the arms have animation, set the right animation and play it accoring to the bending state
	AnimationsComponent<SpriteSheetAnimation>::Handle animationComponent(armsComponent->arms.component<AnimationsComponent<SpriteSheetAnimation>>());
	if(animationComponent)
	{
		std::string directionStr;//Find the right animation string, and set the angle
		if(directionComponent->direction == Direction::Left)
			directionStr = " left";
		else if(directionComponent->direction == Direction::Right)
			directionStr = " right";
		//If the animations manager have the required animation
		if(animationComponent->animationsManager.isRegistred("bend"+directionStr))
			animationComponent->animationsManager.setProgress("bend"+directionStr, bowComponent->targetTranslation);
	}

	//If the bow have animation, set the right animation and play it accoring to the bending state
	animationComponent = holdItemComponent->item.component<AnimationsComponent<SpriteSheetAnimation>>();
	if(animationComponent)
	{
		std::string directionStr;//Find the right animation string, and set the angle
		if(directionComponent->direction == Direction::Left)
			directionStr = " left";
		else if(directionComponent->direction == Direction::Right)
			directionStr = " right";
		//If the animations manager have the required animation
		if(animationComponent->animationsManager.isRegistred("bend"+directionStr))
			animationComponent->animationsManager.setProgress("bend"+directionStr, bowComponent->targetTranslation);
	}
	//If there is not a notched arrow, notch antoher one
	if(not bowComponent->notchedArrow.valid() and not quiverComponent->arrows.empty())
	{
		auto notchedArrowIt(quiverComponent->arrows.begin());
		entityx::Entity notchedArrow(notchedArrowIt->first);
		if(not TEST(notchedArrow.valid()))
			return;
		quiverComponent->arrows.erase(notchedArrowIt);
		bowComponent->notchedArrow = notchedArrow;
		BodyComponent::Handle notchedArrowBodyComponent{notchedArrow.component<BodyComponent>()};
		ArrowComponent::Handle notchedArrowComponent{notchedArrow.component<ArrowComponent>()};
		if(not TEST(notchedArrowBodyComponent and notchedArrowComponent))
			return;

		b2World* world{bowBodyComponent->body->GetWorld()};
		//Destroy all joints (e.g. the quiver/arrow joint)
		for(b2JointEdge* jointEdge{notchedArrowBodyComponent->body->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
			world->DestroyJoint(jointEdge->joint);

		//Set the joint
		b2PrismaticJointDef jointDef;
		jointDef.bodyA = bowBodyComponent->body;
		jointDef.bodyB = notchedArrowBodyComponent->body;
		jointDef.localAnchorA = bowComponent->notchedArrowAnchor;
		jointDef.localAnchorB = sftob2(notchedArrowComponent->localFrictionPoint/Context::parameters->pixelByMeter);
		jointDef.localAxisA = {1.f, 0.f};
		jointDef.lowerTranslation = bowComponent->lowerTranslation;
		jointDef.upperTranslation = bowComponent->upperTranslation;
		jointDef.enableLimit = true;
		jointDef.maxMotorForce = 10.f;
		jointDef.motorSpeed = 0.f;
		jointDef.enableMotor = true;
		jointDef.referenceAngle = 0.f;
		bowComponent->notchedArrowJoint = static_cast<b2PrismaticJoint*>(world->CreateJoint(&jointDef));
		notchedArrowComponent->state = ArrowComponent::Notched;
	}
}
