#include <algorithm>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

template <>
void cast<bool>(Data& var)
{
	if(var.which() == 4)//Convert entity to bool
		var = boost::get<entityx::Entity>(var).valid();
	else if(var.which() == 3)//Convert string to bool
		var = boost::get<std::string>(var).size() > 0;
	else if(var.which() == 1)//Convert int to bool
		var = static_cast<bool>(boost::get<int>(var));
	else if(var.which() == 2)//Convert float to bool
		var = static_cast<bool>(boost::get<float>(var));
}

template <>
void cast<int>(Data& var)
{
	if(var.which() == 4)//Convert entity to int
		throw ScriptError("Conversion from entity to integer is not allowed.");
	else if(var.which() == 3)//Convert string to int
		var = std::stoi(boost::get<std::string>(var));
	else if(var.which() == 0)//Convert bool to int
		var = static_cast<int>(boost::get<bool>(var));
	else if(var.which() == 2)//Convert float to int
		var = static_cast<int>(boost::get<float>(var));
}

template <>
void cast<float>(Data& var)
{
	if(var.which() == 4)//Convert entity to float
		throw ScriptError("Conversion from entity to floating point is not allowed.");
	else if(var.which() == 3)//Convert string to float
		var = std::stof(boost::get<std::string>(var));
	else if(var.which() == 0)//Convert bool to float
		var = static_cast<float>(boost::get<bool>(var));
	else if(var.which() == 1)//Convert int to float
		var = static_cast<float>(boost::get<int>(var));
}

template <>
void cast<std::string>(Data& var)
{
	if(var.which() == 1)//Convert int to string
		var = std::to_string(boost::get<int>(var));
	else if(var.which() == 0)//Convert bool to string
	{
		if(boost::get<bool>(var))
			var = "true";
		else
			var = "false";
	}
	else if(var.which() == 2)//Convert float to string
		var = std::to_string(boost::get<float>(var));
	else if(var.which() == 4)//Convert entity to string
		throw ScriptError("Conversion from entity to string is not allowed.");
}

template <>
void cast<entityx::Entity>(Data& var)
{
	if(var.which() != 4)
		throw ScriptError("Conversion from any type to entity is not allowed.");
}

Data print(const std::vector<Data>& args)
{
	std::cout << std::boolalpha;
	for(size_t i(0); i < args.size(); ++i)
		std::cout << args[i] << " ";
	std::cout << std::endl;
	return 0;
}

Data lowerThanOp(const std::vector<Data>& args)
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

Data greaterThanOp(const std::vector<Data>& args)
{
	return lowerThanOp({args[1], args[0]});
}

Data lowerEqualOp(const std::vector<Data>& args)
{
	return notOp({greaterThanOp(args)});
}

Data greaterEqualOp(const std::vector<Data>& args)
{
	return notOp({lowerThanOp(args)});
}

Data equalOp(const std::vector<Data>& args)
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
		return andOp({greaterEqualOp(args), lowerEqualOp(args)});
	else
		return false;
}

Data notEqualOp(const std::vector<Data>& args)
{
	return notOp({equalOp(args)});
}

Data andOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	cast<bool>(lhs);
	cast<bool>(rhs);
	return boost::get<bool>(lhs) and boost::get<bool>(rhs);
}

Data orOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	cast<bool>(lhs);
	cast<bool>(rhs);
	return boost::get<bool>(lhs) or boost::get<bool>(rhs);
}

Data addOp(const std::vector<Data>& args)
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

Data substractOp(const std::vector<Data>& args)
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

Data multiplyOp(const std::vector<Data>& args)
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

Data divideOp(const std::vector<Data>& args)
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

Data moduloOp(const std::vector<Data>& args)
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
		return std::fmod(boost::get<float>(lhs), boost::get<float>(rhs));
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

Data notOp(const std::vector<Data>& args)
{
	Data lhs(args[0]);
	cast<bool>(lhs);
	return not boost::get<bool>(lhs);
}

entityx::Entity nearestFoe(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(not self)
		throw ScriptError("nearest foe(): first argument is an invalid entity.");
	BodyComponent::Handle bodyComponent(self.component<BodyComponent>());
	DetectionRangeComponent::Handle detectionRangeComponent(self.component<DetectionRangeComponent>());
	if(bodyComponent and detectionRangeComponent)
	{
		const float range{detectionRangeComponent->detectionRange/Context::parameters->pixelByMeter};
		b2Body* body{bodyComponent->body};
		b2World* world{body->GetWorld()};
		NearestFoeQueryCallback callback(self);
		b2AABB aabb;
		aabb.lowerBound = b2Vec2(-range, -range) + body->GetWorldCenter();
		aabb.upperBound = b2Vec2(range, range) + body->GetWorldCenter();
		world->QueryAABB(&callback, aabb);
		return callback.entity;
	}
	return entityx::Entity();
}

float distanceFrom(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	entityx::Entity target(boost::get<entityx::Entity>(args[1]));
	if(not self)
		throw ScriptError("distance from(): first argument is an invalid entity.");
	if(not target)
		throw ScriptError("distance from(): second argument is an invalid entity.");
	if(self == target)
		return 0.f;
	BodyComponent::Handle selfBodyComponent(self.component<BodyComponent>());
	BodyComponent::Handle targetBodyComponent(target.component<BodyComponent>());
	if(selfBodyComponent and targetBodyComponent)
	{
		b2Body* selfBody{selfBodyComponent->body};
		b2Fixture* selfFixture{nullptr};
		//Check all fixtures untill a main fixture is found.
		for(b2Fixture* fixture{selfBody->GetFixtureList()}; fixture and not selfFixture; fixture = fixture->GetNext())
			if(fixtureHasRole(fixture, FixtureRole::Main))
				selfFixture = fixture;

		b2Body* targetBody{targetBodyComponent->body};
		b2Fixture* targetFixture{nullptr};
		//Check all fixtures untill a main fixture is found.
		for(b2Fixture* fixture{targetBody->GetFixtureList()}; fixture and not targetFixture; fixture = fixture->GetNext())
			if(fixtureHasRole(fixture, FixtureRole::Main))
				targetFixture = fixture;

		if(selfFixture and targetFixture)
		{
			b2DistanceProxy selfProxy;
			selfProxy.Set(selfFixture->GetShape(), 1);
			b2DistanceProxy targetProxy;
			targetProxy.Set(targetFixture->GetShape(), 1);

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
	return 0.f;
}

int directionTo(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	entityx::Entity target(boost::get<entityx::Entity>(args[1]));
	if(not self)
		throw ScriptError("direction to(): first argument is an invalid entity.");
	if(not target)
		throw ScriptError("direction to(): second argument is an invalid entity.");
	TransformComponent::Handle selfTransformComponent(self.component<TransformComponent>());
	TransformComponent::Handle targetTransformComponent(target.component<TransformComponent>());
	if(selfTransformComponent and targetTransformComponent)
	{
		if(selfTransformComponent->transform.x < targetTransformComponent->transform.x)
			return static_cast<int>(Direction::Right);
		else
			return static_cast<int>(Direction::Left);
	}
	return static_cast<int>(Direction::None);
}

int directionOf(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(not self)
		throw ScriptError("direction of(): first argument is an invalid entity.");
	DirectionComponent::Handle directionComponent(self.component<DirectionComponent>());
	if(directionComponent)
		return static_cast<int>(directionComponent->direction);
	return static_cast<int>(Direction::None);
}

int attack(const std::vector<Data>& args)
{
	Command attackCommand;
	attackCommand.targetIsSpecific = true;
	attackCommand.entity = boost::get<entityx::Entity>(args[0]);
	if(not attackCommand.entity)
		throw ScriptError("can move(): first argument is an invalid entity.");
	attackCommand.action = HandToHand();
	Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(attackCommand);
	return 0;
}

NearestFoeQueryCallback::NearestFoeQueryCallback(entityx::Entity self):
	entity(),
	m_self(self),
	m_distance(std::numeric_limits<float>::infinity())
{}

bool NearestFoeQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity currentEntity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(not TEST(currentEntity))
		return true;
	CategoryComponent::Handle categoryComponent(currentEntity.component<CategoryComponent>());
	if(categoryComponent and currentEntity.component<CategoryComponent>()->category.test(Category::Passive))//currentEntity must be passive
	{
		if(currentEntity != m_self)
		{
			float currentDistance{distanceFrom({m_self, currentEntity})};
			if(currentDistance < m_distance)
			{
				m_distance = currentDistance;
				entity = currentEntity;
			}
		}
	}
	return true;
}

bool canMove(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(not self)
		throw ScriptError("can move(): first argument is an invalid entity.");
	return self.has_component<WalkComponent>();
}

int move(const std::vector<Data>& args)
{
	Command moveCommand;
	moveCommand.targetIsSpecific = true;
	moveCommand.entity = boost::get<entityx::Entity>(args[0]);
	if(not moveCommand.entity)
		throw ScriptError("can move(): first argument is an invalid entity.");
	moveCommand.action = Mover(static_cast<Direction>(boost::get<int>(args[1])));
	Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(moveCommand);
	return 0;
}

int stop(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(not self)
		throw ScriptError("stop(): first argument is an invalid entity.");
	const DirectionComponent::Handle directionComponent(self.component<DirectionComponent>());
	const WalkComponent::Handle walkComponent(self.component<WalkComponent>());
	if(directionComponent and walkComponent)
	{
		if(directionComponent->moveToLeft and directionComponent->moveToRight)
		{
			Command moveCommand1;
			moveCommand1.targetIsSpecific = true;
			moveCommand1.entity = self;
			Command moveCommand2;
			moveCommand2.targetIsSpecific = true;
			moveCommand2.entity = self;
			moveCommand1.action = Mover(not directionComponent->direction, false);
			moveCommand2.action = Mover(directionComponent->direction, false);
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(moveCommand1);
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(moveCommand2);
		}
		else
		{
			Command moveCommand;
			moveCommand.targetIsSpecific = true;
			moveCommand.entity = self;
			moveCommand.action = Mover(directionComponent->direction, false);
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(moveCommand);
		}
	}
	return 0;
}

bool canJump(const std::vector<Data>& args)
{
	entityx::Entity self(boost::get<entityx::Entity>(args[0]));
	if(not self)
		throw ScriptError("can jump(): first argument is an invalid entity.");
	const FallComponent::Handle fallComponent(self.component<FallComponent>());
	return self.has_component<JumpComponent>() and fallComponent and not fallComponent->inAir;
}

int jump(const std::vector<Data>& args)
{
	if(canJump(args))
	{
		//Simply push a jump command on the command queue
		Command jumpCommand;
		jumpCommand.targetIsSpecific = true;
		jumpCommand.entity = boost::get<entityx::Entity>(args[0]);
		jumpCommand.action = Jumper();
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(jumpCommand);
	}
	return 0;
}
