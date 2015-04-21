#include <algorithm>
#include <iostream>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Collision/b2Distance.h>
#include <entityx/System.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/FixtureRoles.h>

#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

Data print(const std::vector<Data>& args, StateStack::Context)
{
	std::cout << std::boolalpha;
	for(size_t i(0); i < args.size(); ++i)
		std::cout << args[i] << " ";
	std::cout << std::endl;
	return 0;
}

Data lowerThanOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() != 3 and rhs.which() != 3 and lhs.which() != 4 and rhs.which() != 4)
	{
		if(lhs.which() != 2 and rhs.which() != 2)
		{
			cast<int>(lhs);
			cast<int>(rhs);
			return boost::get<int>(lhs) < boost::get<int>(rhs);
		}
		else
		{
			cast<float>(lhs);
			cast<float>(rhs);
			return boost::get<float>(lhs) < boost::get<float>(rhs);
		}
	}
	else if(lhs.which() == 3 and rhs.which() == 3)
		return boost::get<std::string>(lhs).size() < boost::get<std::string>(rhs).size();
	else
		throw std::runtime_error("invalid operands types for comparaison.");
}

Data greaterThanOp(const std::vector<Data>& args, StateStack::Context context)
{
	return lowerThanOp({args[1], args[0]}, context);
}

Data lowerEqualOp(const std::vector<Data>& args, StateStack::Context context)
{
	return notOp({greaterThanOp(args, context)}, context);
}

Data greaterEqualOp(const std::vector<Data>& args, StateStack::Context context)
{
	return notOp({lowerThanOp(args, context)}, context);
}

Data equalOp(const std::vector<Data>& args, StateStack::Context context)
{
	const Data& lhs(args[0]), rhs(args[1]);
	//Both are string
	if(lhs.which() == 3 and rhs.which() == 3)
		return boost::get<std::string>(lhs) == boost::get<std::string>(rhs);
	//Left is entity, right is bool
	else if(lhs.which() == 4 and rhs.which() == 0)
		return boost::get<entityx::Entity>(lhs).valid() == boost::get<bool>(rhs);
	//Left is bool, right is entity
	else if(lhs.which() == 0 and rhs.which() == 4)
		return boost::get<bool>(lhs) == boost::get<entityx::Entity>(rhs).valid();
	//Both are numeric
	else if(lhs.which() <= 2 and rhs.which() <= 2)
		return andOp({greaterEqualOp(args, context), lowerEqualOp(args, context)}, context);
	else
		return false;
}

Data notEqualOp(const std::vector<Data>& args, StateStack::Context context)
{
	return notOp({equalOp(args, context)}, context);
}

Data andOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	cast<bool>(lhs);
	cast<bool>(rhs);
	return boost::get<bool>(lhs) and boost::get<bool>(rhs);
}

Data orOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	cast<bool>(lhs);
	cast<bool>(rhs);
	return boost::get<bool>(lhs) or boost::get<bool>(rhs);
}

Data addOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() == 4 or rhs.which() == 4)
		throw ScriptError("entities does not support addition.");
	else if(lhs.which() == 3 or rhs.which() == 3)
	{
		cast<std::string>(lhs);
		cast<std::string>(rhs);
		return boost::get<std::string>(lhs) + boost::get<std::string>(rhs);
	}
	else if(lhs.which() == 2 or rhs.which() == 2)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return boost::get<float>(lhs) + boost::get<float>(rhs);
	}
	else if(lhs.which() == 1 or rhs.which() == 1)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return boost::get<int>(lhs) + boost::get<int>(rhs);
	}
	else
		return static_cast<bool>(boost::get<bool>(lhs) + boost::get<bool>(rhs));
}

Data substractOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() == 4 or rhs.which() == 4)
		throw ScriptError("entities does not support substraction.");
	else if(lhs.which() == 3 or rhs.which() == 3)
		throw std::runtime_error("string does not support substraction");
	else if(lhs.which() == 2 or rhs.which() == 2)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return boost::get<float>(lhs) - boost::get<float>(rhs);
	}
	else if(lhs.which() == 1 or rhs.which() == 1)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return boost::get<int>(lhs) - boost::get<int>(rhs);
	}
	else
		return static_cast<bool>(boost::get<bool>(lhs) + boost::get<bool>(rhs));
}

Data multiplyOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() == 4 or rhs.which() == 4)
		throw ScriptError("entities does not support multiplication.");
	else if(lhs.which() == 3 or rhs.which() == 3)
		throw std::runtime_error("string does not support multiplication.");
	else if(lhs.which() == 2 or rhs.which() == 2)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return boost::get<float>(lhs) * boost::get<float>(rhs);
	}
	else if(lhs.which() == 1 or rhs.which() == 1)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return boost::get<int>(lhs) * boost::get<int>(rhs);
	}
	else
		return static_cast<bool>(boost::get<bool>(lhs) * boost::get<bool>(rhs));
}

Data divideOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() == 4 or rhs.which() == 4)
		throw ScriptError("entities does not support division.");
	else if(lhs.which() == 3 or rhs.which() == 3)
		throw std::runtime_error("string does not support division.");
	else if(lhs.which() == 2 or rhs.which() == 2)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		if(boost::get<float>(rhs) == 0.f)
			throw std::runtime_error("division by zero");
		return boost::get<float>(lhs) / boost::get<float>(rhs);
	}
	else if(lhs.which() == 1 or rhs.which() == 1)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		if(boost::get<int>(rhs) == 0)
			throw std::runtime_error("division by zero");
		return boost::get<int>(lhs) / boost::get<int>(rhs);
	}
	else
		throw std::runtime_error("boolean does not support division.");
}

Data moduloOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.which() == 4 or rhs.which() == 4)
		throw ScriptError("entities does not support modulo.");
	else if(lhs.which() == 3 or rhs.which() == 3)
		throw std::runtime_error("string does not support modulo.");
	else if(lhs.which() == 2 or rhs.which() == 2)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		if(boost::get<float>(rhs) == 0.f)
			throw std::runtime_error("modulo by zero");
		return static_cast<float>(fmod(boost::get<float>(lhs), boost::get<float>(rhs)));
	}
	else if(lhs.which() == 1 or rhs.which() == 1)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		if(boost::get<int>(rhs) == 0)
			throw std::runtime_error("modulo by zero");
		return boost::get<int>(lhs) % boost::get<int>(rhs);
	}
	else
		throw std::runtime_error("boolean does not support modulo.");
}

Data notOp(const std::vector<Data>& args, StateStack::Context)
{
	Data lhs(args[0]);
	cast<bool>(lhs);
	return not boost::get<bool>(lhs);
}

entityx::Entity nearestFoe(const std::vector<Data>& args, StateStack::Context context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(self.has_component<BodyComponent>() and self.has_component<DetectionRangeComponent>())
	{
		auto& bodies(self.component<BodyComponent>()->bodies);
		if(bodies.find("main") != bodies.end())
		{
			float range{self.component<DetectionRangeComponent>()->detectionRange/context.parameters.pixelByMeter};
			b2Body* body{bodies["main"]};
            b2World* world{body->GetWorld()};
            NearestFoeQueryCallback callback(self, context);
			b2AABB aabb;
			aabb.lowerBound = b2Vec2(-range, -range) + body->GetWorldCenter();
			aabb.upperBound = b2Vec2(range, range) + body->GetWorldCenter();
			world->QueryAABB(&callback, aabb);
			return callback.entity;
		}
	}
	return entityx::Entity();
}

float distanceFrom(const std::vector<Data>& args, StateStack::Context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	entityx::Entity target(boost::get<entityx::Entity>(args[1]));
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

int directionTo(const std::vector<Data>& args, StateStack::Context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	entityx::Entity target(boost::get<entityx::Entity>(args[1]));
	if(self and target and self.has_component<TransformComponent>() and target.has_component<TransformComponent>())
	{
		if(self.component<TransformComponent>()->transforms["main"].x < target.component<TransformComponent>()->transforms["main"].x)
			return static_cast<int>(Direction::Right);
		else
			return static_cast<int>(Direction::Left);
	}
	return static_cast<int>(Direction::None);
}

int attack(const std::vector<Data>&, StateStack::Context)
{
	return 0;
}

NearestFoeQueryCallback::NearestFoeQueryCallback(entityx::Entity self, StateStack::Context context):
	entity(),
	m_self(self),
	m_distance(std::numeric_limits<float>::infinity()),
	m_context(context)
{}

bool NearestFoeQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity currentEntity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(m_self.has_component<CategoryComponent>()
			and currentEntity.has_component<CategoryComponent>()
			and m_self.component<CategoryComponent>()->category & Category::Aggressive//self must be aggressive
			and currentEntity.component<CategoryComponent>()->category & Category::Passive)//currentEntity must be passive
	{
		float currentDistance{distanceFrom({m_self, currentEntity}, m_context)};
		if(currentEntity != m_self and currentDistance < m_distance)
		{
			m_distance = currentDistance;
			entity = currentEntity;
		}
	}
	return true;
}

bool canMove(const std::vector<Data>& args, StateStack::Context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	return self and self.has_component<WalkComponent>();
}

int move(const std::vector<Data>& args, StateStack::Context context)
{
	Command moveCommand;
	moveCommand.targetIsSpecific = true;
	moveCommand.entity = boost::get<entityx::Entity>(args[0]);
	moveCommand.action = Mover(static_cast<Direction>(boost::get<int>(args[1])));
	context.systemManager.system<PendingChangesSystem>()->commandQueue.push(moveCommand);
	return 0;
}

int stop(const std::vector<Data>& args, StateStack::Context context)
{
	Command moveCommand;
	moveCommand.targetIsSpecific = true;
	moveCommand.entity = boost::get<entityx::Entity>(args[0]);
	moveCommand.action = Mover(static_cast<Direction>(boost::get<int>(args[1])), false);
	context.systemManager.system<PendingChangesSystem>()->commandQueue.push(moveCommand);
	return 0;
}

bool canJump(const std::vector<Data>& args, StateStack::Context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	return self
		and self.has_component<JumpComponent>()
		and self.has_component<FallComponent>()
		and not self.component<FallComponent>()->inAir;
}

int jump(const std::vector<Data>& args, StateStack::Context context)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(canJump({self}, context))
	{
		//Simply push a jump command on the command queue
		Command jumpCommand;
		jumpCommand.targetIsSpecific = true;
		jumpCommand.entity = self;
		jumpCommand.action = Jumper();
		context.systemManager.system<PendingChangesSystem>()->commandQueue.push(jumpCommand);
	}
	return 0;
}