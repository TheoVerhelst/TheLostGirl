#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/JointRoles.h>

#include <TheLostGirl/actions.h>

Mover::Mover(Direction _direction, bool _start):
	direction{_direction},
	start{_start}
{}

Mover::~Mover()
{}
		
void Mover::operator()(entityx::Entity entity, double) const
{
	std::string directionStr;
	std::string oppDirectionStr;
	Direction oppDirection;
	bool moveIsHorizontal;
	switch(direction)
	{
		case Direction::Left:
			directionStr = " left";
			oppDirectionStr = " right";
			oppDirection = Direction::Right;
			moveIsHorizontal = true;
			break;
		
		case Direction::Right:
			directionStr = " right";
			oppDirectionStr = " left";
			oppDirection = Direction::Left;
			moveIsHorizontal = true;
			break;
		
		case Direction::Top:
			directionStr = "Top";
			oppDirectionStr = "Bottom";
			oppDirection = Direction::Bottom;
			moveIsHorizontal = false;
			break;
		
		case Direction::Bottom:
			directionStr = "Bottom";
			oppDirectionStr = "Top";
			oppDirection = Direction::Top;
			moveIsHorizontal = false;
			break;
		
		default:
			return;
			break;
	}
	if(moveIsHorizontal)
	{
		//For all entities
		if(entity.has_component<DirectionComponent>())
		{
			DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
			//References to the moveToLeft and moveToRight data in directionComponent
			bool& moveToDirection(direction == Direction::Left ? directionComponent->moveToLeft : directionComponent->moveToRight);
			bool& moveToOppDirection(direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
			if(start)
			{
				moveToDirection = true;
				//Here in all cases, the new direction will be the same.
				directionComponent->direction = direction;
			}
			else
			{
				moveToDirection = false;
				if(moveToOppDirection)
					directionComponent->direction = oppDirection;
			}
		}
		//If the entity can walk, set the right animation.
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
			and entity.has_component<WalkComponent>()
			and entity.has_component<DirectionComponent>())
		{
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
			WalkComponent::Handle walkComponent{entity.component<WalkComponent>()};
			//References to the moveToLeft and moveToRight data in directionComponent
			bool moveToOppDirection{direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight};
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animations
				if(animations.isRegistred("stay" + directionStr)
					and animations.isRegistred("stay" + oppDirectionStr)
					and animations.isRegistred("move" + directionStr)
					and animations.isRegistred("move" + oppDirectionStr))
				{
					if(start)
					{
						//Force to set the right animations
						animations.stop("stay" + oppDirectionStr);
						animations.play("stay" + directionStr);
						animations.stop("move" + oppDirectionStr);
						animations.play("move" + directionStr);
						if(moveToOppDirection)
						{
							//Stop the player
							walkComponent->effectiveMovement = Direction::None;//Stop the player
							animations.stop("move" + directionStr);
						}
						else
							walkComponent->effectiveMovement = direction;
					}
					else
					{
						animations.stop("move" + directionStr);
						if(moveToOppDirection)
						{
							//Force to play the opposite direction animations
							animations.play("move" + oppDirectionStr);
							animations.stop("stay" + directionStr);
							animations.play("stay" + oppDirectionStr);
							walkComponent->effectiveMovement = oppDirection;
						}
						else
							walkComponent->effectiveMovement = Direction::None;//Stop the player
					}
				}
			}
		}
		//If the entity can jump, set the right animation if it jumps
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
			and entity.has_component<JumpComponent>()
			and entity.has_component<FallComponent>()
			and entity.has_component<DirectionComponent>())
		{
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animations
				if(animations.isRegistred("jump" + directionStr)
					and animations.isRegistred("jump" + oppDirectionStr))
				{
					//If jumping and diriged to the opposite side
					if(animations.isActive("jump" + oppDirectionStr))
					{
						float progress{animations.getProgress("jump" + oppDirectionStr)};
						animations.stop("jump" + oppDirectionStr);
						animations.play("jump" + directionStr);
						animations.setProgress("jump" + directionStr, progress);
					}
				}
			}
		}
		//If the entity can fall, set the right animation if it falls
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
			and entity.has_component<FallComponent>()
			and entity.has_component<DirectionComponent>())
		{
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animations
				if(animations.isRegistred("fall" + directionStr)
					and animations.isRegistred("fall" + oppDirectionStr))
				{
					//If falling and diriged to the opposite side
					if(animations.isActive("fall" + oppDirectionStr))
					{
						float progress{animations.getProgress("fall" + oppDirectionStr)};
						animations.stop("fall" + oppDirectionStr);
						animations.play("fall" + directionStr);
						animations.setProgress("fall" + directionStr, progress);
					}
				}
			}
		}
		//If the entity can bend a bow, set the right animation if it bends
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
			and entity.has_component<BendComponent>()
			and entity.has_component<DirectionComponent>())
		{
			DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
			bool moveToOppDirection{direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight};
			//Get all the animations managers of the entity
			auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
			//For each animations manager of the entity
			for(auto& animationsPair : animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have the required animations
				if(animations.isRegistred("bend" + directionStr)
					and animations.isRegistred("bend" + oppDirectionStr))
				{
					if(start)
					{
						//If the player is diriged to the other side, and the bending is currently actived
						if(animations.isActive("bend" + oppDirectionStr) and entity.component<BendComponent>()->power > 0.f)
						{
							//Flip the angle
							if(direction == Direction::Left)
								entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi, b2_pi/2, 2*b2_pi);
							else if(direction == Direction::Right)
								entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi/2, b2_pi, 2*b2_pi);
						}
						float progress{animations.getProgress("bend" + oppDirectionStr)};
						animations.stop("bend" + oppDirectionStr);
						animations.activate("bend" + directionStr);
						animations.setProgress("bend" + directionStr, progress);
					}
					else if(moveToOppDirection)
					{
						float progress{animations.getProgress("bend" + directionStr)};
						//If the bending is currently actived
						if(entity.component<BendComponent>()->power > 0.f)
						{
							//Flip the angle
							if(direction == Direction::Left)
								entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi, b2_pi/2, 2*b2_pi);
							else if(direction == Direction::Right)
								entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi/2, b2_pi, 2*b2_pi);
						}
						animations.stop("bend" + directionStr);
						animations.activate("bend" + oppDirectionStr);
						animations.setProgress("bend" + oppDirectionStr, progress);
					}
				}
			}
		}
	}
}

Jumper::Jumper()
{}

Jumper::~Jumper()
{}

void Jumper::operator()(entityx::Entity entity, double) const
{
	if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
		and entity.has_component<BodyComponent>()
		and entity.has_component<JumpComponent>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{
		JumpComponent::Handle jumpComponent{entity.component<JumpComponent>()};
		FallComponent::Handle fallComponent{entity.component<FallComponent>()};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//Get all the animations managers of the entity
		auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
		//Get all the bodies of the entity
		std::map<std::string, b2Body*>& bodies(entity.component<BodyComponent>()->bodies);
		//For each animations manager of the entity
		for(auto& animationsPair : animationsManagers)
		{
			AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
			//If the animations manager have the required animations
			if(animations.isRegistred("jump left") and animations.isRegistred("jump right")
				and bodies.find(animationsPair.first) != bodies.end())
			{
				if(not fallComponent->inAir)
				{
					float targetVelocity{-jumpComponent->jumpStrength};
					b2Body* body{bodies[animationsPair.first]};
					float mass{body->GetMass()};
					body->ApplyLinearImpulse({0.f, targetVelocity*mass}, body->GetWorldCenter(), true);
					if(directionComponent->direction == Direction::Left)
						animations.play("jump left");
					else if(directionComponent->direction == Direction::Right)
						animations.play("jump right");
				}
			}
		}
	}
}

BowBender::BowBender(float _angle, float _power):
	angle{_angle},
	power{_power}
{}

BowBender::~BowBender()
{}

void BowBender::operator()(entityx::Entity entity, double) const
{
	if(entity.has_component<BendComponent>() and entity.has_component<DirectionComponent>() and entity.has_component<BodyComponent>())
	{
		BendComponent::Handle bendComponent{entity.component<BendComponent>()};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//Set the bending angle
		if(directionComponent->direction == Direction::Left)
			bendComponent->angle = cap(angle - b2_pi, -b2_pi, b2_pi/2);
		else if(directionComponent->direction == Direction::Right)
			bendComponent->angle = cap(angle, -b2_pi/2, b2_pi);
		//Set the bending power
		bendComponent->power = cap(power, 0.f, bendComponent->maxPower);
		
		//If the entity has animation, set the right animation and play it accoring to the bending state
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>())
		{
			std::string directionStr;//Find the right animation string, and set the angle
			if(directionComponent->direction == Direction::Left)
				directionStr = " left";
			else if(directionComponent->direction == Direction::Right)
				directionStr = " right";
			float animationPower{bendComponent->power / bendComponent->maxPower};//The progress of the bending, in the range [0, 1]
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
		//If the entity has a quiver
		if(entity.has_component<QuiverComponent>())
		{
			QuiverComponent::Handle quiverComponent{entity.component<QuiverComponent>()};
			entityx::Entity notchedArrow{quiverComponent->notchedArrow};
			//If there is not a notched arrow
			if(not notchedArrow.valid())
			{
				//Find the first valid arrow in the quiver
				auto found = std::find_if(quiverComponent->arrows.begin(), quiverComponent->arrows.end(), [](const entityx::Entity& e)
												{return e.valid();});
				if(found != quiverComponent->arrows.end())
				{
					quiverComponent->notchedArrow = *found;
					notchedArrow = *found;
					
					//If the notched arrow has not a main body, the program will crash
					assert(notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end());
					b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
					//If the entity has not a bow, the program will crash
					assert(entity.component<BodyComponent>()->bodies.find("bow") != entity.component<BodyComponent>()->bodies.end());
					b2Body* bowBody{entity.component<BodyComponent>()->bodies["bow"]};
					
					//Replace the arrow
					arrowBody->SetTransform(bowBody->GetWorldPoint({76.f, 93.f}), bowBody->GetAngle());
					//Set the joint
					b2PrismaticJointDef jointDef;
					jointDef.bodyA = bowBody;
					jointDef.bodyB = arrowBody;
					jointDef.localAnchorA = {76.f, 93.f};
					jointDef.localAnchorB = {0.f, 6.f};
					jointDef.localAxisA = {1.f, 0.f};
					jointDef.enableLimit = true;
					jointDef.upperTranslation = 0.f;
					jointDef.lowerTranslation = -1.f;
					jointDef.enableMotor = true;
					jointDef.maxMotorForce = 10.f;
					jointDef.referenceAngle = 0.f;
					jointDef.userData = add<unsigned int>(jointDef.userData, static_cast<unsigned int>(JointRole::BendingPower));
				}
			}
			
			//If the notched arrow has a b2Body,
			//Set the translation of the joint between bow and arrow
			if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>())
			{
				//If the notched arrow has not a main body, the program will crash
				assert(notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end());
				b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
				float translationTarget{bendComponent->power/bendComponent->maxPower};//Power of the bending, in range [0, 1]
				float32 gain{10.f};
				float32 translationError;
				//Iterate over all joints
				for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				{
					//If this is a bending translation joint
					if(jointHasRole(jointEdge->joint, JointRole::BendingPower))
					{
						b2PrismaticJoint* joint{static_cast<b2PrismaticJoint*>(jointEdge->joint)};
						//The final target is equal to the joint's lower limit when the translationTarget is equal to 1.
						translationError = joint->GetJointTranslation() - (translationTarget*joint->GetLowerLimit());
						joint->SetMotorSpeed(-gain * translationError);
					}
				}
			}
		}
	}
}
