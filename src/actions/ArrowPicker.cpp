#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <TheLostGirl/components.h>

#include <TheLostGirl/actions/ArrowPicker.h>

ArrowPicker::ArrowPicker()
{
}

ArrowPicker::~ArrowPicker()
{
}

void ArrowPicker::operator()(entityx::Entity entity, double) const
{
	if(not entity.valid())
		return;
	const auto directionComponent(entity.component<DirectionComponent>());
	const auto bodyComponent(entity.component<BodyComponent>());
	auto bowComponent(entity.component<BowComponent>());
	if(bodyComponent and directionComponent and bowComponent)
	{
		auto bodyIt(bodyComponent->bodies.find("main"));
		if(bodyIt != bodyComponent->bodies.end())
		{
			b2World* world{bodyIt->second->GetWorld()};

			//Do the querying
			b2AABB pickBox;
			pickBox.lowerBound = bodyIt->second->GetWorldCenter() - b2Vec2(2, 2);
			pickBox.upperBound = bodyIt->second->GetWorldCenter() + b2Vec2(2, 2);
			StickedArrowQueryCallback callback;
			world->QueryAABB(&callback, pickBox);

			if(callback.foundEntity.valid())
			{
				b2Body* arrowBody{callback.foundEntity.component<BodyComponent>()->bodies.at("main")};

				//Destroy all joints (e.g. the ground/arrow weld joint)
				for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
					arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

				//Set the joint
				b2WeldJointDef jointDef;
				jointDef.bodyA = bodyIt->second;
				jointDef.bodyB = arrowBody;
				if(directionComponent->direction == Direction::Left)
				{
					jointDef.referenceAngle = -b2_pi/2.f;
					jointDef.localAnchorA = {0.36666667f, 0.49166667f};
					jointDef.localAnchorB = {0.4f, 0.10833333f};
				}
				else if(directionComponent->direction == Direction::Right)
				{
					jointDef.referenceAngle = -b2_pi/2.f;
					jointDef.localAnchorA = {0.36666667f, 0.49166667f};
					jointDef.localAnchorB = {0.4f, 0.10833333f};
				}
				jointDef.frequencyHz = 0.f;
				jointDef.dampingRatio = 0.f;
				world->CreateJoint(&jointDef);

				//Add the arrow to the quiver
				bowComponent->arrows.push_back(callback.foundEntity);
				callback.foundEntity.component<ArrowComponent>()->state = ArrowComponent::Stored;
				callback.foundEntity.component<ArrowComponent>()->shooter = entity;
			}
		}
	}
}

bool StickedArrowQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	//Return false (and so stop) only if this is a arrow and if this one is sticked.
	const auto arrowComponent(entity.component<ArrowComponent>());
	bool found{arrowComponent and arrowComponent->state == ArrowComponent::Sticked};
	if(found)
		foundEntity = entity;
	return not found;
}
