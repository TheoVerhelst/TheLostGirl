#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/actions/BowBender.h>

BowBender::BowBender(float _angle, float _power):
	angle{_angle},
	power{_power}
{
}

BowBender::~BowBender()
{
}

void BowBender::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	ArcherComponent::Handle archerComponent{entity.component<ArcherComponent>()};
	ArticuledArmsComponent::Handle armsComponent{entity.component<ArticuledArmsComponent>()};
	BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
	DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
	if(not (archerComponent and archerComponent->quiver.valid() and bodyComponent and directionComponent
			and armsComponent and armsComponent->arms.valid()))
		return;
	HoldItemComponent::Handle holdItemComponent{armsComponent->arms.component<HoldItemComponent>()};
	if(not (holdItemComponent and holdItemComponent->item.valid()))
		return;
	BowComponent::Handle bowComponent{holdItemComponent->item.component<BowComponent>()};
	if(not bowComponent)
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
	QuiverComponent::Handle quiverComponent{archerComponent->quiver.component<QuiverComponent>()};
	//If there is not a notched arrow, notch antoher one
	if(not bowComponent->notchedArrow.valid() and not quiverComponent->arrows.empty())
	{
		BodyComponent::Handle arrowBodyComponent;
		entityx::Entity notchedArrow{quiverComponent->arrows.back()};
		quiverComponent->arrows.pop_back();
		bowComponent->notchedArrow = notchedArrow;
		b2Body* arrowBody{notchedArrow.component<BodyComponent>()->body};
		b2Body* bowBody{holdItemComponent->item.component<BodyComponent>()->body};

		b2World* world{bowBody->GetWorld()};
		//Destroy all joints (e.g. the quiver/arrow joint)
		for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
			world->DestroyJoint(jointEdge->joint);

		//Set the joint
		b2PrismaticJointDef jointDef;
		jointDef.bodyA = bowBody;
		jointDef.bodyB = arrowBody;
		//Get the AABB of the arrow, to compute where place anchor of the joint
		b2AABB arrowBox;
		b2Transform identity;
		identity.SetIdentity();
		jointDef.localAnchorA = bowComponent->localAnchor;
		for(b2Fixture* fixture{arrowBody->GetFixtureList()}; fixture; fixture = fixture->GetNext())
			if(fixtureHasRole(fixture, FixtureRole::Main))
				fixture->GetShape()->ComputeAABB(&arrowBox, identity, 0);
		jointDef.localAnchorB = {0.f, (arrowBox.upperBound.y - arrowBox.lowerBound.y)*0.5f};
		jointDef.localAxisA = {1.f, 0.f};
		jointDef.lowerTranslation = bowComponent->lowerTranslation;
		jointDef.upperTranslation = bowComponent->upperTranslation;
		jointDef.enableLimit = true;
		jointDef.maxMotorForce = 10.f;
		jointDef.motorSpeed = 0.f;
		jointDef.enableMotor = true;
		jointDef.referenceAngle = 0.f;
		bowComponent->notchedArrowJoint = static_cast<b2PrismaticJoint*>(world->CreateJoint(&jointDef));
		notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Notched;
	}
}
