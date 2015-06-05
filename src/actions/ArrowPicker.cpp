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
	if(not entity)
		return;
	if(entity.has_component<BodyComponent>()
		and entity.has_component<DirectionComponent>()
		and entity.has_component<BowComponent>())
	{
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
		if(bodyComponent->bodies.count("main"))
		{
			b2Body* body{bodyComponent->bodies.at("main")};
			b2World* world{body->GetWorld()};

			//Do the querying
			b2AABB pickBox;
			pickBox.lowerBound = body->GetWorldCenter() - b2Vec2(2, 2);
			pickBox.upperBound = body->GetWorldCenter() + b2Vec2(2, 2);
			StickedArrowQueryCallback callback;
			world->QueryAABB(&callback, pickBox);

			if(callback.foundEntity.valid())
			{
				b2Body* arrowBody{callback.foundEntity.component<BodyComponent>()->bodies.at("main")};
				b2Body* characterBody{entity.component<BodyComponent>()->bodies.at("main")};

				//Destroy all joints (e.g. the ground/arrow weld joint)
				for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
					arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

				//Set the joint
				b2WeldJointDef jointDef;
				jointDef.bodyA = characterBody;
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
				entity.component<BowComponent>()->arrows.push_back(callback.foundEntity);
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
	bool found{entity.has_component<ArrowComponent>() and entity.component<ArrowComponent>()->state == ArrowComponent::Sticked};
	if(found)
		foundEntity = entity;
	return not found;
}
