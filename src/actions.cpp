#include <Box2D/Dynamics/b2Body.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/actions.h>

Mover::Mover(Direction _direction, bool _start):
	direction{_direction},
	start{_start}
{}

Mover::~Mover()
{}
		
void Mover::operator()(entityx::Entity& entity, double) const
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
			DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
			//References to the moveToLeft and moveToRight data in directionComponent
			bool& moveToDirection = (direction == Direction::Left ? directionComponent->moveToLeft : directionComponent->moveToRight);
			bool& moveToOppDirection = (direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
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
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<WalkComponent>()
			and entity.has_component<DirectionComponent>())
		{
			Animations* animations = entity.component<AnimationsComponent>()->animations;
			DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
			WalkComponent::Handle walkComponent = entity.component<WalkComponent>();
			//References to the moveToLeft and moveToRight data in directionComponent
			bool moveToOppDirection = (direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
			if(start)
			{
				//Force to set the right animations
				animations->stop("stay" + oppDirectionStr);
				animations->play("stay" + directionStr);
				animations->stop("move" + oppDirectionStr);
				animations->play("move" + directionStr);
				if(moveToOppDirection)
				{
					//Stop the player
					walkComponent->effectiveMovement = Direction::None;//Stop the player
					animations->stop("move" + directionStr);
				}
				else
					walkComponent->effectiveMovement = direction;
			}
			else
			{
				animations->stop("move" + directionStr);
				if(moveToOppDirection)
				{
					//Force to play the opposite direction animations
					animations->play("move" + oppDirectionStr);
					animations->stop("stay" + directionStr);
					animations->play("stay" + oppDirectionStr);
					walkComponent->effectiveMovement = oppDirection;
				}
				else
					walkComponent->effectiveMovement = Direction::None;//Stop the player
			}
		}
		//If the entity can jump, set the right animation if it jumps
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<JumpComponent>()
			and entity.has_component<FallComponent>()
			and entity.has_component<DirectionComponent>())
		{
			Animations* animations = entity.component<AnimationsComponent>()->animations;
			//If jumping and diriged to the opposite side
			if(animations->isActive("jump" + oppDirectionStr))
			{
				float progress = animations->getProgress("jump" + oppDirectionStr);
				animations->stop("jump" + oppDirectionStr);
				animations->play("jump" + directionStr);
				animations->setProgress("jump" + directionStr, progress);
			}
		}
		//If the entity can fall, set the right animation if it falls
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<FallComponent>()
			and entity.has_component<DirectionComponent>())
		{
			Animations* animations = entity.component<AnimationsComponent>()->animations;
			//If falling and diriged to the opposite side
			if(animations->isActive("fall" + oppDirectionStr))
			{
				float progress = animations->getProgress("fall" + oppDirectionStr);
				animations->stop("fall" + oppDirectionStr);
				animations->play("fall" + directionStr);
				animations->setProgress("fall" + directionStr, progress);
			}
		}
		//If the entity can bend a bow, set the right animation if it bends
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<BendComponent>()
			and entity.has_component<DirectionComponent>())
		{
			Animations* animations = entity.component<AnimationsComponent>()->animations;
//			//If falling and diriged to the opposite side
//			if(animations->isActive("bend" + oppDirectionStr))
//			{
//				float progress = animations->getProgress("bend" + oppDirectionStr);
//				animations->stop("bend" + oppDirectionStr);
//				animations->activate("bend" + directionStr);
//				animations->setProgress("bend" + directionStr, progress);
//				entity.component<BendComponent>()->angle = remainder(entity.component<BendComponent>()->angle, b2_pi) - b2_pi;
//			}
			DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
			bool moveToOppDirection = (direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
			if(start)
			{
				float progress = animations->getProgress("bend" + oppDirectionStr);
				//If a second key is pressed when the other is held
				if(animations->isActive("bend" + oppDirectionStr))
				{
					//Flip the angle
					if(direction == Direction::Left)
						entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi, b2_pi/2, 2*b2_pi);
					else if(direction == Direction::Right)
						entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi/2, b2_pi, 2*b2_pi);
				}
				animations->stop("bend" + oppDirectionStr);
				animations->activate("bend" + directionStr);
				animations->setProgress("bend" + directionStr, progress);
			}
			else if(moveToOppDirection)
			{
				float progress = animations->getProgress("bend" + directionStr);
				//Flip the angle
				if(direction == Direction::Left)
					entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi, b2_pi/2, 2*b2_pi);
				else if(direction == Direction::Right)
					entity.component<BendComponent>()->angle = cap(entity.component<BendComponent>()->angle - b2_pi, -b2_pi/2, b2_pi, 2*b2_pi);
				animations->stop("bend" + directionStr);
				animations->activate("bend" + oppDirectionStr);
				animations->setProgress("bend" + oppDirectionStr, progress);
			}
		}
	}
}

Jumper::Jumper()
{}

Jumper::~Jumper()
{}

void Jumper::operator()(entityx::Entity& entity, double) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<BodyComponent>()
		and entity.has_component<JumpComponent>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		b2Body* body = entity.component<BodyComponent>()->body;
		JumpComponent::Handle jumpComponent = entity.component<JumpComponent>();
		FallComponent::Handle fallComponent = entity.component<FallComponent>();
		DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
		if(not fallComponent->inAir)
		{
			body->SetLinearVelocity({body->GetLinearVelocity().x, -jumpComponent->jumpStrength});
			if(directionComponent->direction == Direction::Left)
				animations->play("jump left");
			else if(directionComponent->direction == Direction::Right)
				animations->play("jump right");
		}
	}
}

BowBender::BowBender(float _angle, float _power):
	angle{_angle},
	power{_power}
{}

BowBender::~BowBender()
{}

void BowBender::operator()(entityx::Entity& entity, double) const
{
	if(entity.has_component<BendComponent>()
		and entity.has_component<AnimationsComponent>()
		and entity.has_component<DirectionComponent>())
	{
		BendComponent::Handle bendComponent = entity.component<BendComponent>();
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
		
		std::string directionStr;//Find the right animation string, and set the angle
		if(directionComponent->direction == Direction::Left)
		{
			directionStr = " left";
			bendComponent->angle = cap(angle - b2_pi, -b2_pi, b2_pi/2);
		}
		else if(directionComponent->direction == Direction::Right)
		{
			directionStr = " right";
			bendComponent->angle = cap(angle, -b2_pi/2, b2_pi);
		}
		
		bendComponent->power = cap(power, 0.f, bendComponent->maxPower);//Cap the power
		float animationPower = bendComponent->power / bendComponent->maxPower;//The progress of the bending, in the range [0, 1]
		animations->setProgress("bend"+directionStr, animationPower);
		std::cout << "bend"+directionStr << " at " << bendComponent->angle << " radians." << std::endl;
	}
}