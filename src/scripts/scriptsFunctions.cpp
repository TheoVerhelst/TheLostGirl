#include <algorithm>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>

#include <TheLostGirl/scripts/scriptsFunctions.h>

entityx::Entity nearestFoe(entityx::Entity self, float pixelByMeter)
{
	if(self.has_component<BodyComponent>() and self.has_component<DetectionRangeComponent>())
	{
		auto& bodies(self.component<BodyComponent>()->bodies);
		if(bodies.find("main") != bodies.end())
		{
			float range{self.component<DetectionRangeComponent>()->detectionRange/pixelByMeter};
			b2Body* body{bodies["main"]};
            b2World* world{body->GetWorld()};
            NearestEntityQueryCallback callback(self);
			b2AABB aabb;
			aabb.lowerBound.Set(-range, -range);
			aabb.upperBound.Set(range, range);
			world->QueryAABB(&callback, aabb);
			return callback.entity;
		}
	}
	return entityx::Entity();
}

float distanceFrom(entityx::Entity self, entityx::Entity target)
{
	if(self == target)
		return 0;
	if(self.has_component<BodyComponent>() and target.has_component<BodyComponent>())
	{
		auto& selfBodies(self.component<BodyComponent>()->bodies);
		auto& targetBodies(target.component<BodyComponent>()->bodies);
		if(selfBodies.find("main") != selfBodies.end() and targetBodies.find("main") != targetBodies.end())
		{
			b2Body* selfBody{selfBodies["main"]};
			b2Fixture* selfFixture{nullptr};
		}
	}
	return 100.f;
}

int directionTo(entityx::Entity self, entityx::Entity target)
{
	return 100;
}

int attack(entityx::Entity, entityx::Entity)
{
	return 0;
}

NearestEntityQueryCallback::NearestEntityQueryCallback(entityx::Entity self):
	entity(),
	m_self(self),
	m_distance(0)
{}

bool NearestEntityQueryCallback::ReportFixture(b2Fixture* fixture)
{
	b2Body* body{fixture->GetBody()};
	entityx::Entity current_entity{*static_cast<entityx::Entity*>(body->GetUserData())};
	float current_distance = distanceFrom(m_self, current_entity);
	//If this is not the entity that look around itself, and if the current entity is
	//the nearest one we handle for now, or if it is the first entity found.
	if(current_entity != m_self and (current_distance < m_distance or not entity))
	{
		m_distance = current_distance;
		entity = current_entity;
	}
	return true;
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
	if(canJump(self))
	{
		//Simply push a jump command on the command queue
		Command jumpCommand;
		jumpCommand.targetIsSpecific = true;
		jumpCommand.entity = self;
		jumpCommand.action = Jumper();
		commandQueue.push(jumpCommand);
	}
	return 0;
}
