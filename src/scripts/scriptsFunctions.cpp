#include <Box2D/Dynamics/b2Body.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>

#include <TheLostGirl/scripts/scriptsFunctions.h>

entityx::Entity nearestFoe(entityx::Entity self)
{
	if(self.has_component<BodyComponent>())
	{
		auto& bodies(self.component<BodyComponent>()->bodies);
		if(bodies.find("main") != bodies.end())
		{
			b2Body* body{bodies["main"]};
            b2World* world{body->GetWorld()};
            //Get all bodies, select bodies that are foes, and select the nearest, and get the entity.

		}
	}
	return entityx::Entity();
}

float distanceFrom(entityx::Entity, entityx::Entity)
{
	return 100.f;
}

int directionTo(entityx::Entity, entityx::Entity)
{
	return 100;
}

int attack(entityx::Entity, entityx::Entity)
{
	return 0;
}

bool canMove(entityx::Entity)
{
	return true;
}

int move(entityx::Entity, int)
{
	return 0;
}

int stop(entityx::Entity)
{
	return 0;
}

bool canJump(entityx::Entity self)
{
	return self.has_component<JumpComponent>()
		and self.has_component<FallComponent>()
		and not self.component<FallComponent>()->inAir;
}

int jump(entityx::Entity self, std::queue<Command>& commandQueue)
{
	//Simply push a jump command on the command queue
	Command jumpCommand;
	jumpCommand.targetIsSpecific = true;
	jumpCommand.entity = self;
	jumpCommand.action = Jumper();
	commandQueue.push(jumpCommand);
	return 0;
}
