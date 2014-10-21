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
		and entity.has_component<Direction>())
	{
		AnimationsComponent::Handle animationsComponent = entity.component<AnimationsComponent>();
		b2Body* body = entity.component<Body>()->body;
		Walk::Handle walkComponent = entity.component<Walk>();
		Direction::Handle direction = entity.component<Direction>();
		float speed = walkComponent->walkSpeed;
		if(start)
		{
			walkComponent->moveToLeft = true;
			if(walkComponent->moveToRight)
			{
				animationsComponent->animations->play("stayLeft");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				direction->toLeft = true;
			}
			else
			{
				animationsComponent->animations->play("moveLeft");
				body->SetLinearVelocity({-speed, body->GetLinearVelocity().y});
				direction->toLeft = true;
			}
		}
		else
		{
			walkComponent->moveToLeft = false;
			if(walkComponent->moveToRight)
			{
				animationsComponent->animations->play("moveRight");
				body->SetLinearVelocity({speed, body->GetLinearVelocity().y});
				direction->toLeft = false;
			}
			else
			{
				animationsComponent->animations->play("stayLeft");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				direction->toLeft = true;
			}
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
		and entity.has_component<Direction>())
	{
		AnimationsComponent::Handle animationsComponent = entity.component<AnimationsComponent>();
		b2Body* body = entity.component<Body>()->body;
		Walk::Handle walkComponent = entity.component<Walk>();
		Direction::Handle direction = entity.component<Direction>();
		float speed = walkComponent->walkSpeed;
		if(start)
		{
			walkComponent->moveToRight = true;
			if(walkComponent->moveToLeft)
			{
				animationsComponent->animations->play("stayRight");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				direction->toLeft = false;
			}
			else
			{
				animationsComponent->animations->play("moveRight");
				body->SetLinearVelocity({speed, body->GetLinearVelocity().y});
				direction->toLeft = false;
			}
		}
		else
		{
			walkComponent->moveToRight = false;
			if(walkComponent->moveToLeft)
			{
				animationsComponent->animations->play("moveLeft");
				body->SetLinearVelocity({-speed, body->GetLinearVelocity().y});
				direction->toLeft = true;
			}
			else
			{
				animationsComponent->animations->play("stayRight");
				body->SetLinearVelocity({0, body->GetLinearVelocity().y});
				direction->toLeft = false;
			}
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
		and entity.has_component<Direction>())
	{
		Animations* animations = entity.component<AnimationsComponent>()->animations;
		b2Body* body = entity.component<Body>()->body;
		Jump::Handle jumpComponent = entity.component<Jump>();
		Direction::Handle direction = entity.component<Direction>();
		if(not jumpComponent->isJumping)
		{
			body->SetLinearVelocity({body->GetLinearVelocity().x, jumpComponent->jumpStrength});
			if(direction->toLeft)
				animations->play("jumpLeft");
			else
				animations->play("jumpRight");
			jumpComponent->isJumping = true;
		}
	}
}