#include <entityx/Entity.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/JointRoles.h>

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
			bowComponent->angle = cap(angle - b2_pi, -b2_pi, b2_pi/2);
		else if(directionComponent->direction == Direction::Right)
			bowComponent->angle = cap(angle, -b2_pi/2, b2_pi);
		//Set the bending power
		bowComponent->power = cap(power, 0.f, bowComponent->maxPower);

		//If the entity has animation, set the right animation and play it accoring to the bending state
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>())
		{
			std::string directionStr;//Find the right animation string, and set the angle
			if(directionComponent->direction == Direction::Left)
				directionStr = " left";
			else if(directionComponent->direction == Direction::Right)
				directionStr = " right";
			float animationPower{bowComponent->power / bowComponent->maxPower};//The progress of the bending, in the range [0, 1]
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animation
				if(animations.isRegistred("bend"+directionStr))
					animations.setProgress("bend"+directionStr, animationPower);
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
					if(directionComponent->direction == Direction::Left)
					{
						jointDef.referenceAngle = b2_pi;
						jointDef.localAnchorA = {0.183333f, 0.41666667f};
						jointDef.localAnchorB = {0.4f-0.025f, 0.05f};
					}
					else if(directionComponent->direction == Direction::Right)
					{
						jointDef.referenceAngle = 0.f;
						jointDef.localAnchorA = {0.625f, 0.41666667f};
						jointDef.localAnchorB = {0.025f, 0.05f};
					}
					jointDef.localAxisA = {1.f, 0.f};
					jointDef.enableLimit = true;
					jointDef.lowerTranslation = -0.30833333f;
					jointDef.upperTranslation = 0.f;
					jointDef.enableMotor = true;
					jointDef.maxMotorForce = 10.f;
					jointDef.userData = add<unsigned int>(jointDef.userData, static_cast<unsigned int>(JointRole::BendingPower));
					b2World* world{bowBody->GetWorld()};
					world->CreateJoint(&jointDef);
					notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Notched;
				}
			}
		}
	}
}