#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/actions/ArrowPicker.h>

void ArrowPicker::operator()(entityx::Entity entity) const
{
	if(not TEST(entity.valid()))
		return;
	const BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	ArcherComponent::Handle archerComponent(entity.component<ArcherComponent>());
	if(not TEST(bodyComponent and archerComponent and archerComponent->quiver))
		return;
	BodyComponent::Handle quiverBodyComponent(archerComponent->quiver.component<BodyComponent>());
	QuiverComponent::Handle quiverComponent{archerComponent->quiver.component<QuiverComponent>()};
	if(not TEST(quiverBodyComponent and quiverComponent))
		return;
	b2World* world{bodyComponent->body->GetWorld()};
	//Do the querying
	b2AABB pickBox;
	pickBox.lowerBound = bodyComponent->body->GetWorldCenter() - b2Vec2(2, 2);
	pickBox.upperBound = bodyComponent->body->GetWorldCenter() + b2Vec2(2, 2);
	StickedArrowQueryCallback callback;
	world->QueryAABB(&callback, pickBox);

	if(callback.foundEntity.valid())
	{
		b2Body* arrowBody{callback.foundEntity.component<BodyComponent>()->body};
		ArrowComponent::Handle arrowComponent{callback.foundEntity.component<ArrowComponent>()};

		//Destroy all joints (e.g. the ground/arrow weld joint)
		for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
			arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

		//Set the joint
		b2WeldJointDef jointDef;
		jointDef.bodyA = quiverBodyComponent->body;
		jointDef.bodyB = arrowBody;
		jointDef.localAnchorA = quiverComponent->arrowAnchor;
		jointDef.localAnchorB = sftob2(arrowComponent->localStickPoint/Context::parameters->pixelByMeter);
		jointDef.referenceAngle = -b2_pi/2.f;
		jointDef.frequencyHz = 0.f;
		jointDef.dampingRatio = 0.f;

		//Add the arrow to the quiver
		quiverComponent->arrows.emplace(callback.foundEntity, static_cast<b2WeldJoint*>(world->CreateJoint(&jointDef)));
		arrowComponent->state = ArrowComponent::Stored;
		arrowComponent->shooter = entity;
	}
}

bool StickedArrowQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(not TEST(entity.valid()))
		return true;
	//Return false (and so stop) only if this is a arrow and if it is sticked or is not moving
	const ArrowComponent::Handle arrowComponent(entity.component<ArrowComponent>());
	const BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	bool found{arrowComponent and bodyComponent and (arrowComponent->state == ArrowComponent::Sticked or bodyComponent->body->GetLinearVelocity().LengthSquared() < 0.001f)};
	if(found)
		foundEntity = entity;
	return not found;
}
