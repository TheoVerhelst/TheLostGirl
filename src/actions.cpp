#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
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
					entity.component<BowComponent>()->angle = cap(remainder(entity.component<BowComponent>()->angle - b2_pi, 2*b2_pi), -b2_pi, b2_pi/2);
				else if(directionComponent->direction == Direction::Right)
					entity.component<BowComponent>()->angle = cap(remainder(entity.component<BowComponent>()->angle - b2_pi, 2*b2_pi), -b2_pi/2, b2_pi);
				
				//If the entity has a quiver
				if(entity.has_component<BowComponent>())
				{
					BowComponent::Handle bowComponent{entity.component<BowComponent>()};
					entityx::Entity notchedArrow{bowComponent->notchedArrow};
					//If the notched arrow has a b2Body
					if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>())
					{
						//If the notched arrow has not a main body, the program will crash
						assert(notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end());
						b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
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
									jointDef.localAnchorA = {0.183333, 0.41666667};
									jointDef.localAnchorB = {0.4f-0.025f, 0.05f};
								}
								else if(directionComponent->direction == Direction::Right)
								{
									jointDef.referenceAngle = 0.f;
									jointDef.localAnchorA = {0.625f, 0.41666667};
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
	}
}

Jumper::Jumper()
{}

Jumper::~Jumper()
{}

void Jumper::operator()(entityx::Entity entity, double) const
{
	if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
		and entity.has_component<JumpComponent>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{
		FallComponent::Handle fallComponent{entity.component<FallComponent>()};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//Get all the animations managers of the entity
		auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
		//For each animations manager of the entity
		for(auto& animationsPair : animationsManagers)
		{
			AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
			//If the animations manager have the required animations
			if(animations.isRegistred("jump left") and animations.isRegistred("jump right"))
			{
				if(not fallComponent->inAir)
				{
					entity.component<JumpComponent>()->mustJump = true;//The physics system will do the physic jump job
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
				
				//If the notched arrow has not a main body, the program will crash
				assert(notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end());
				b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
				//If the entity has not a bow, the program will crash
				assert(entity.component<BodyComponent>()->bodies.find("bow") != entity.component<BodyComponent>()->bodies.end());
				b2Body* bowBody{entity.component<BodyComponent>()->bodies["bow"]};
				
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
					jointDef.localAnchorA = {0.183333, 0.41666667};
					jointDef.localAnchorB = {0.4f-0.025f, 0.05f};
				}
				else if(directionComponent->direction == Direction::Right)
				{
					jointDef.referenceAngle = 0.f;
					jointDef.localAnchorA = {0.625f, 0.41666667};
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

ArrowShooter::ArrowShooter()
{}

ArrowShooter::~ArrowShooter()
{}

void ArrowShooter::operator()(entityx::Entity entity, double) const
{
	if(entity.has_component<BowComponent>()
		and entity.has_component<DirectionComponent>())
	{
		BowComponent::Handle bowComponent{entity.component<BowComponent>()};
		entityx::Entity notchedArrow{bowComponent->notchedArrow};
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		//If the notched arrow has a b2Body
		if(notchedArrow.valid() and notchedArrow.has_component<BodyComponent>() and notchedArrow.has_component<ArrowComponent>())
		{
			//If the notched arrow has not a main body, the program will crash
			assert(notchedArrow.component<BodyComponent>()->bodies.find("main") != notchedArrow.component<BodyComponent>()->bodies.end());
			b2Body* arrowBody{notchedArrow.component<BodyComponent>()->bodies["main"]};
				
			//Destroy all joints (e.g. the bow/arrow joint)
			for(b2JointEdge* jointEdge{arrowBody->GetJointList()}; jointEdge; jointEdge = jointEdge->next)
				arrowBody->GetWorld()->DestroyJoint(jointEdge->joint);
			
			double shootForceX{bowComponent->power*cos(bowComponent->angle)};
			double shootForceY{-bowComponent->power*sin(bowComponent->angle)};
			if(directionComponent->direction == Direction::Left)
			{
				shootForceX = bowComponent->power*cos(bowComponent->angle+b2_pi);
				shootForceY = -bowComponent->power*sin(bowComponent->angle+b2_pi);
			}
			b2Vec2 shootForce{static_cast<float32>(shootForceX), static_cast<float32>(shootForceY)};
			arrowBody->ApplyLinearImpulse((arrowBody->GetMass()/20.f)*shootForce, arrowBody->GetWorldCenter(), true);
			bowComponent->notchedArrow = entityx::Entity();
			notchedArrow.component<ArrowComponent>()->state = ArrowComponent::Fired;
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

ArrowPicker::ArrowPicker()
{}

ArrowPicker::~ArrowPicker()
{}

void ArrowPicker::operator()(entityx::Entity entity, double) const
{
	if(entity.has_component<BodyComponent>()
		and entity.has_component<DirectionComponent>()
		and entity.has_component<BowComponent>())
	{
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
		//If the entity has not a main body, the program will crash
		assert(bodyComponent->bodies.find("main") != bodyComponent->bodies.end());
		b2Body* body{bodyComponent->bodies["main"]};
		b2World* world{body->GetWorld()};
		
		//Do the querying
		b2AABB pickBox;
		pickBox.lowerBound = body->GetWorldCenter() - b2Vec2(2, 2);
		pickBox.upperBound = body->GetWorldCenter() + b2Vec2(2, 2);
		StickedArrowQueryCallback callback;
		world->QueryAABB(&callback, pickBox);
		
		if(callback.foundEntity.valid())
		{
			//If the found arrow has not a main body, the program will crash
			assert(callback.foundEntity.component<BodyComponent>()->bodies.find("main") != callback.foundEntity.component<BodyComponent>()->bodies.end());
			b2Body* arrowBody{callback.foundEntity.component<BodyComponent>()->bodies["main"]};
			//If the entity has not a main body, the program will crash
			assert(entity.component<BodyComponent>()->bodies.find("main") != entity.component<BodyComponent>()->bodies.end());
			b2Body* characterBody{entity.component<BodyComponent>()->bodies["main"]};
			
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
		}
	}
}
