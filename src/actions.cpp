#include <Box2D/Dynamics/b2Body.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

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
			directionStr = "Left";
			oppDirectionStr = "Right";
			oppDirection = Direction::Right;
			moveIsHorizontal = true;
			break;
		
		case Direction::Right:
			directionStr = "Right";
			oppDirectionStr = "Left";
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
		//If the entity can walk, set the right animation.
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<Body>()
			and entity.has_component<Walk>()
			and entity.has_component<DirectionComponent>())
		{
			Animations* animations = entity.component<AnimationsComponent>()->animations;
			b2Body* body = entity.component<Body>()->body;
			float walkSpeed = entity.component<Walk>()->walkSpeed;
			DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
			//References to the moveToLeft and moveToRight data in directionComponent
			bool& moveToDirection = (direction == Direction::Left ? directionComponent->moveToLeft : directionComponent->moveToRight);
			bool& moveToOppDirection = (direction == Direction::Right ? directionComponent->moveToLeft : directionComponent->moveToRight);
			float xVelocity = walkSpeed * (direction == Direction::Left ? -1 : 1);
			float yVelocity = body->GetLinearVelocity().y;
			if(start)
			{
				moveToDirection = true;
				//Force to set the right animations
				animations->stop("stay" + oppDirectionStr);
				animations->play("stay" + directionStr);
				animations->stop("move" + oppDirectionStr);
				animations->play("move" + directionStr);
				//Here in all cases, the new direction will be the same.
				directionComponent->direction = direction;
				if(moveToOppDirection)
				{
					//Stop the player
					animations->stop("move" + directionStr);
					body->SetLinearVelocity({0, yVelocity});
				}
				else
					body->SetLinearVelocity({xVelocity, yVelocity});
			}
			else
			{
				moveToDirection = false;
				animations->stop("move" + directionStr);
				if(moveToOppDirection)
				{
					//Force to play the opposite direction animations
					animations->play("move" + oppDirectionStr);
					animations->stop("stay" + directionStr);
					animations->play("stay" + oppDirectionStr);
					body->SetLinearVelocity({-xVelocity, yVelocity});
					directionComponent->direction = oppDirection;
				}
				else
					body->SetLinearVelocity({0, yVelocity});//Stop the player
			}
		}
		//If the entity can jump, set the right animation if it jumps
		if(entity.has_component<AnimationsComponent>()
			and entity.has_component<Jump>()
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
	}
}

Jumper::Jumper()
{}

Jumper::~Jumper()
{}

void Jumper::operator()(entityx::Entity& entity, double) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Body>()
		and entity.has_component<Jump>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		b2Body* body = entity.component<Body>()->body;
		Jump::Handle jumpComponent = entity.component<Jump>();
		FallComponent::Handle fallComponent = entity.component<FallComponent>();
		DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
		if(not fallComponent->inAir)
		{
			body->SetLinearVelocity({body->GetLinearVelocity().x, jumpComponent->jumpStrength});
			if(directionComponent->direction == Direction::Left)
				animations->play("jumpLeft");
			else if(directionComponent->direction == Direction::Right)
				animations->play("jumpRight");
		}
	}
}

BowBender::BowBender(bool _start):
	start{_start}
{}

BowBender::~BowBender()
{}

void BowBender::operator()(entityx::Entity&, double) const
{}