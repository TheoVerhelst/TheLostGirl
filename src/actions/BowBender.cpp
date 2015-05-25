#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/b2Fixture.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/JointRoles.h>
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
	if(entity.has_component<BowComponent>() and entity.has_component<DirectionComponent>() and entity.has_component<BodyComponent>())
	{
		BowComponent::Handle bowComponent{entity.component<BowComponent>()};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//Set the bending angle
		if(directionComponent->direction == Direction::Left)
			bowComponent->angle = cap(angle - b2_pi, -b2_pi, b2_pi/2.f);
		else if(directionComponent->direction == Direction::Right)
			bowComponent->angle = cap(angle, -b2_pi/2.f, b2_pi);
		//Set the bending power, limited to 500px of drag and drop
		bowComponent->power = cap(power, 0.f, 500.f)/500.f;

		//If the entity has animation, set the right animation and play it accoring to the bending state
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>())
		{
			std::string directionStr;//Find the right animation string, and set the angle
			if(directionComponent->direction == Direction::Left)
				directionStr = " left";
			else if(directionComponent->direction == Direction::Right)
				directionStr = " right";
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animation
				if(animations.isRegistred("bend"+directionStr))
					animations.setProgress("bend"+directionStr, bowComponent->power);
			}
		}
		//Notch an arrow if there is no one
		entityx::Entity notchedArrow{bowComponent->notchedArrow};
		//If there is not a notched arrow
		if(not notchedArrow.valid())
		{
			//Find the first valid arrow in the quiver
			auto found = std::find_if(bowComponent->arrows.begin(), bowComponent->arrows.end(),
										[](const entityx::Entity& e){return e.valid();});
			if(found != bowComponent->arrows.end())
			{
				bowComponent->notchedArrow = *found;
				notchedArrow = *found;
				bowComponent->arrows.erase(found);
				auto& arrowBodies(notchedArrow.component<BodyComponent>()->bodies);
				auto& entityBodies(entity.component<BodyComponent>()->bodies);
				if(arrowBodies.find("main") != arrowBodies.end() and entityBodies.find("bow") != entityBodies.end())
				{
					b2Body* arrowBody{arrowBodies.at("main")};
					b2Body* bowBody{entityBodies.at("bow")};

					//Destroy all joints (e.g. the quiver/arrow joint)
					for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
						arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);

					//Set the joint
					b2PrismaticJointDef jointDef;
					jointDef.bodyA = bowBody;
					jointDef.bodyB = arrowBody;
					//Get the AABB of the arrow, to compute where place anchor of the joint
					b2AABB arrowBox;
					b2Transform identity;
					identity.SetIdentity();
					for(b2Fixture* fixture{arrowBody->GetFixtureList()}; fixture; fixture = fixture->GetNext())
						if(fixtureHasRole(fixture, FixtureRole::Main))
							fixture->GetShape()->ComputeAABB(&arrowBox, identity, 0);
					b2Vec2 arrowSize{arrowBox.upperBound - arrowBox.lowerBound};
					if(directionComponent->direction == Direction::Left)
					{
						jointDef.referenceAngle = b2_pi;
						jointDef.localAnchorA = {-0.1f, 0.41f};
						jointDef.localAnchorB = {arrowSize.x, arrowSize.y*0.5f};
					}
					else if(directionComponent->direction == Direction::Right)
					{
						jointDef.referenceAngle = 0.f;
						jointDef.localAnchorA = {0.625f, 0.41f};
						jointDef.localAnchorB = {0.f, arrowSize.y*0.5f};
					}
					jointDef.localAxisA = {1.f, 0.f};
					jointDef.enableLimit = true;
					jointDef.lowerTranslation = -0.3333333f;
					jointDef.upperTranslation = 0.f;
					jointDef.enableMotor = true;
					jointDef.maxMotorForce = 10.f;
					jointDef.userData = add(jointDef.userData, static_cast<long unsigned int>(JointRole::BendingPower));
					b2World* world{bowBody->GetWorld()};
					world->CreateJoint(&jointDef);
					notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Notched;
				}
			}
		}
	}
}
