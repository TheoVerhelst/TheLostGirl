#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/actions/ArrowPicker.hpp>

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
	
	std::set<entityx::Entity> foundEntities{Box2DHelper::queryEntities(pickBox, isPickableArrow)};

	if(not foundEntities.empty())
	{
		entityx::Entity foundEntity{*foundEntities.begin()};
		b2Body* arrowBody{foundEntity.component<BodyComponent>()->body};
		ArrowComponent::Handle arrowComponent{foundEntity.component<ArrowComponent>()};

		//Destroy all joints (e.g. the ground/arrow weld joint)
		for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
			arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

		//Set the joint
		b2WeldJointDef jointDef;
		jointDef.bodyA = quiverBodyComponent->body;
		jointDef.bodyB = arrowBody;
		jointDef.localAnchorA = quiverComponent->arrowAnchor;
		jointDef.localAnchorB = sftob2(arrowComponent->localStickPoint/Context::getParameters().pixelByMeter);
		jointDef.referenceAngle = -b2_pi/2.f;
		jointDef.frequencyHz = 0.f;
		jointDef.dampingRatio = 0.f;

		//Add the arrow to the quiver
		quiverComponent->arrows.emplace(foundEntity, static_cast<b2WeldJoint*>(world->CreateJoint(&jointDef)));
		arrowComponent->state = ArrowComponent::Stored;
		arrowComponent->shooter = entity;
	}
}

bool ArrowPicker::isPickableArrow(entityx::Entity entity)
{
	//Return true only if this is a arrow and if it is sticked or is not moving
	const ArrowComponent::Handle arrowComponent(entity.component<ArrowComponent>());
	const BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	
	return arrowComponent and bodyComponent
		and (arrowComponent->state == ArrowComponent::Sticked
		or bodyComponent->body->GetLinearVelocity().LengthSquared() < m_speedThreshold);
}
