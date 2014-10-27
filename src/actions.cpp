#include <Box2D/Dynamics/b2Body.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/actions.h>

LeftMover::LeftMover(bool _start):
	start(_start)
{}

LeftMover::~LeftMover()
{}

void LeftMover::operator()(entityx::Entity& entity, double dt) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Body>()
		and entity.has_component<Walk>()
		and entity.has_component<DirectionComponent>())
	{
		AnimationsComponent::Handle animationsComponent = entity.component<AnimationsComponent>();
		b2Body* body = entity.component<Body>()->body;
		Walk::Handle walkComponent = entity.component<Walk>();
		DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
		float speed = walkComponent->walkSpeed;
		if(start)
		{
			directionComponent->moveToLeft = true;
			animationsComponent->animations->stop("stayRight");
			animationsComponent->animations->play("stayLeft");
			animationsComponent->animations->stop("moveRight");
			animationsComponent->animations->play("moveLeft");
			if(directionComponent->moveToRight)
			{
				animationsComponent->animations->stop("moveLeft");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Left;
			}
			else
			{
				body->SetLinearVelocity({-speed, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Left;
			}
		}
		else
		{
			directionComponent->moveToLeft = false;
			animationsComponent->animations->stop("moveLeft");
			if(directionComponent->moveToRight)
			{
				animationsComponent->animations->play("moveRight");
				animationsComponent->animations->stop("stayLeft");
				animationsComponent->animations->play("stayRight");
				body->SetLinearVelocity({speed, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Right;
			}
			else
			{
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Left;
			}
		}
	}
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Jump>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{//For every entity that can jump, set the right animation if it goes up
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		if(animations->isActive("fallRight"))
		{//If falling and diriged to left
			float progress = animations->getProgress("fallRight");
			animations->stop("fallRight");
			animations->play("fallLeft");
			animations->setProgress("fallLeft", progress);
		}
		if(animations->isActive("jumpRight"))
		{//If jumping and diriged to left
			float progress = animations->getProgress("jumpRight");
			animations->stop("jumpRight");
			animations->play("jumpLeft");
			animations->setProgress("jumpLeft", progress);
		}
	}
}

RightMover::RightMover(bool _start):
	start(_start)
{}

RightMover::~RightMover()
{}

void RightMover::operator()(entityx::Entity& entity, double dt) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Body>()
		and entity.has_component<Walk>()
		and entity.has_component<DirectionComponent>())
	{
		AnimationsComponent::Handle animationsComponent = entity.component<AnimationsComponent>();
		b2Body* body = entity.component<Body>()->body;
		Walk::Handle walkComponent = entity.component<Walk>();
		DirectionComponent::Handle directionComponent = entity.component<DirectionComponent>();
		float speed = walkComponent->walkSpeed;
		if(start)
		{
			directionComponent->moveToRight = true;
			animationsComponent->animations->stop("stayLeft");
			animationsComponent->animations->play("stayRight");
			animationsComponent->animations->stop("moveLeft");
			animationsComponent->animations->play("moveRight");
			if(directionComponent->moveToLeft)
			{
				animationsComponent->animations->stop("moveRight");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Right;
			}
			else
			{
				body->SetLinearVelocity({speed, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Right;
			}
		}
		else
		{
			directionComponent->moveToRight = false;
			animationsComponent->animations->stop("moveRight");
			if(directionComponent->moveToLeft)
			{
				animationsComponent->animations->play("moveLeft");
				animationsComponent->animations->stop("stayRight");
				animationsComponent->animations->play("stayLeft");
				body->SetLinearVelocity({-speed, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Left;
			}
			else
			{
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				directionComponent->direction = Direction::Right;
			}
		}
	}
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Jump>()
		and entity.has_component<FallComponent>()
		and entity.has_component<DirectionComponent>())
	{//For every entity that can jump, set the right animation if it goes up
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		if(animations->isActive("fallLeft"))
		{//If jumping and diriged to left
			float progress = animations->getProgress("fallLeft");
			animations->stop("fallLeft");
			animations->play("fallRight");
			animations->setProgress("fallRight", progress);
		}
		if(animations->isActive("jumpLeft"))
		{//If jumping and diriged to left
			float progress = animations->getProgress("jumpLeft");
			animations->stop("jumpLeft");
			animations->play("jumpRight");
			animations->setProgress("jumpRight", progress);
		}
	}
}

UpMover::UpMover(bool _start):
	start(_start)
{}

UpMover::~UpMover()
{}

void UpMover::operator()(entityx::Entity& entity, double dt) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Body>()
		and entity.has_component<Walk>())
	{
		if(start)
			std::cout << "StartMoveUp" << std::endl;
		else
			std::cout << "StopMoveUp" << std::endl;
	}
}

DownMover::DownMover(bool _start):
	start(_start)
{}

DownMover::~DownMover()
{}

void DownMover::operator()(entityx::Entity& entity, double dt) const
{
	if(entity.has_component<AnimationsComponent>()
		and entity.has_component<Body>()
		and entity.has_component<Walk>())
	{
		if(start)
			std::cout << "StartMoveDown" << std::endl;
		else
			std::cout << "StopMoveDown" << std::endl;
	}
}

Jumper::Jumper()
{}

Jumper::~Jumper()
{}

void Jumper::operator()(entityx::Entity& entity, double dt) const
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
	start(_start)
{}

BowBender::~BowBender()
{}

void BowBender::operator()(entityx::Entity&, double) const
{}