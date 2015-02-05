#include <algorithm>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/b2Distance.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/FixtureRoles.h>

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
            NearestFoeQueryCallback callback(self);
			b2AABB aabb;
			aabb.lowerBound = b2Vec2(-range, -range) + body->GetWorldCenter();
			aabb.upperBound = b2Vec2(range, range) + body->GetWorldCenter();
			world->QueryAABB(&callback, aabb);
			return callback.entity;
		}
	}
	return entityx::Entity();
}

float distanceFrom(entityx::Entity self, entityx::Entity target)
{
	if(self == target)
		return 0.f;
	if(self.has_component<BodyComponent>() and target.has_component<BodyComponent>())
	{
		auto& selfBodies(self.component<BodyComponent>()->bodies);
		auto& targetBodies(target.component<BodyComponent>()->bodies);
		if(selfBodies.find("main") != selfBodies.end() and targetBodies.find("main") != targetBodies.end())
		{
			b2Body* selfBody{selfBodies["main"]};
			b2Fixture* selfFixture{nullptr};
			//Check all fixtures until a main fixture is found.
			for(b2Fixture* fixture{selfBody->GetFixtureList()}; fixture and not selfFixture; fixture = fixture->GetNext())
				if(fixtureHasRole(fixture, FixtureRole::Main))
					selfFixture = fixture;
			b2DistanceProxy selfProxy;

			b2Body* targetBody{targetBodies["main"]};
			b2Fixture* targetFixture{nullptr};
			//Check all fixtures until a main fixture is found.
			for(b2Fixture* fixture{targetBody->GetFixtureList()}; fixture and not targetFixture; fixture = fixture->GetNext())
				if(fixtureHasRole(fixture, FixtureRole::Main))
					targetFixture = fixture;
			b2DistanceProxy targetProxy;

			if(selfFixture and targetFixture)
			{
				selfProxy.Set(selfFixture->GetShape(),1);
				targetProxy.Set(targetFixture->GetShape(),1);
				b2DistanceInput distanceInput;
				distanceInput.transformA = selfBody->GetTransform();
				distanceInput.transformB = targetBody->GetTransform();
				distanceInput.proxyA = selfProxy;
				distanceInput.proxyB = targetProxy;
				distanceInput.useRadii = false;
				b2SimplexCache cache;
				cache.count = 0;

				b2DistanceOutput distance;
				b2Distance(&distance, &cache, &distanceInput);
				return distance.distance;
			}
		}
	}
	return 0.f;
}

int directionTo(entityx::Entity self, entityx::Entity target)
{
	return 100;
}

int attack(entityx::Entity, entityx::Entity)
{
	return 0;
}

NearestFoeQueryCallback::NearestFoeQueryCallback(entityx::Entity self):
	entity(),
	m_self(self),
	m_distance()
{}

bool NearestFoeQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity currentEntity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(m_self.has_component<CategoryComponent>()
			and currentEntity.has_component<CategoryComponent>()
			and m_self.component<CategoryComponent>()->category & Category::Aggressive//self must be aggressive
			and currentEntity.component<CategoryComponent>()->category & Category::Passive)//currentEntity must be passive
	{
		//If this is not the entity that look around itself, and if the current entity is
		//the nearest one we handle for now, or if it is the first entity found.
		//The weird assignement in condition avoid to run distanceFrom() if entity is equal to true,
		//And to keep the result for the following line.
		float currentDistance;
		if(currentEntity != m_self and (not entity or (currentDistance = distanceFrom(m_self, currentEntity)) < m_distance))
		{
			m_distance = currentDistance;
			entity = currentEntity;
		}
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
	return self
		and self.has_component<JumpComponent>()
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
