#include <entityx/Entity.h>
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/JointRoles.h>

#include <TheLostGirl/actions/Mover.h>

Mover::Mover(Direction _direction, bool _start):
	direction{_direction},
	start{_start}
{
}

Mover::~Mover()
{
}

void Mover::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
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
			directionStr = " top";
			oppDirectionStr = " bottom";
			oppDirection = Direction::Bottom;
			moveIsHorizontal = false;
			break;

		case Direction::Bottom:
			directionStr = " bottom";
			oppDirectionStr = " top";
			oppDirection = Direction::Top;
			moveIsHorizontal = false;
			break;

		case Direction::None:
		default:
			return;
	}
	if(moveIsHorizontal)
	{
		//For all entities
		if(entity.has_component<DirectionComponent>())
		{
			DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
			//References to the moveToLeft or moveToRight data in directionComponent
			bool& moveToDirection(direction == Direction::Left ? directionComponent->moveToLeft : directionComponent->moveToRight);
			bool& moveToOppDirection(direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
			Direction initialDirection{directionComponent->direction};
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
			//Flip the bend componnent if there is one and if the entity has flip
			if(entity.has_component<BowComponent>() and directionComponent->direction != initialDirection)
			{
				//Flip the angle
				if(directionComponent->direction == Direction::Left)
					entity.component<BowComponent>()->angle = cap(std::remainder(entity.component<BowComponent>()->angle - b2_pi, 2.f*b2_pi), -b2_pi, b2_pi/2.f);
				else if(directionComponent->direction == Direction::Right)
					entity.component<BowComponent>()->angle = cap(std::remainder(entity.component<BowComponent>()->angle - b2_pi, 2.f*b2_pi), -b2_pi/2.f, b2_pi);

				//If the entity has a quiver
				if(entity.has_component<BowComponent>())
				{
					BowComponent::Handle bowComponent{entity.component<BowComponent>()};
					entityx::Entity notchedArrow{bowComponent->notchedArrow};
					//If the notched arrow has a b2Body
					if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>()
							and notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end())
					{
						b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies.at("main")};
						//Iterate over all joints
						for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
						{
							//If this is a bending translation joint
							if(jointHasRole(jointEdge->joint, JointRole::BendingPower))
							{
								//Copy the joint and create a new one
								b2PrismaticJoint* joint{static_cast<b2PrismaticJoint*>(jointEdge->joint)};
								b2PrismaticJointDef jointDef;
								jointDef.bodyA = joint->GetBodyA();
								jointDef.bodyB = joint->GetBodyB();
								jointDef.localAxisA = joint->GetLocalAxisA();
								jointDef.enableLimit = joint->IsLimitEnabled();
								jointDef.lowerTranslation = joint->GetLowerLimit();
								jointDef.upperTranslation = joint->GetUpperLimit();
								jointDef.enableMotor = joint->IsMotorEnabled();
								jointDef.maxMotorForce = joint->GetMaxMotorForce();
								jointDef.motorSpeed = joint->GetMotorSpeed();
								jointDef.userData = joint->GetUserData();
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

								b2World* world{jointEdge->joint->GetBodyA()->GetWorld()};
								world->CreateJoint(&jointDef);
								world->DestroyJoint(jointEdge->joint);
							}
						}
					}
				}
			}
		}
		//If the entity have animations
		if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
			and entity.has_component<DirectionComponent>())
		{
			bool moveToOppDirection{direction == Direction::Right ? entity.component<DirectionComponent>()->moveToLeft : entity.component<DirectionComponent>()->moveToRight};
			//For each animations manager of the entity
			for(auto& animationsPair : entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
				//If the animations manager have bending animations
				if(animations.isRegistred("bend" + directionStr)
					and animations.isRegistred("bend" + oppDirectionStr))
				{
					if(start)
					{
						float progress{animations.getProgress("bend" + oppDirectionStr)};
						animations.stop("bend" + oppDirectionStr);
						animations.activate("bend" + directionStr);
						animations.setProgress("bend" + directionStr, progress);
					}
					else if(moveToOppDirection)
					{
						float progress{animations.getProgress("bend" + directionStr)};
						animations.stop("bend" + directionStr);
						animations.activate("bend" + oppDirectionStr);
						animations.setProgress("bend" + oppDirectionStr, progress);
					}
				}
				//If the animations manager falling animations
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
				//If the animations manager have jump animations
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
				//If the animations manager have walk animations
				if(animations.isRegistred("stay" + directionStr)
					and animations.isRegistred("stay" + oppDirectionStr)
					and animations.isRegistred("move" + directionStr)
					and animations.isRegistred("move" + oppDirectionStr))
				{
					WalkComponent::Handle walkComponent{entity.component<WalkComponent>()};
					if(start)
					{
						//Force to set the right animations
						animations.stop("stay" + oppDirectionStr);
						animations.play("stay" + directionStr);
						animations.stop("move" + oppDirectionStr);
						animations.play("move" + directionStr);
						if(moveToOppDirection)
						{
							//Stop the entity
							walkComponent->effectiveMovement = Direction::None;//Stop the entity
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
	}
}
