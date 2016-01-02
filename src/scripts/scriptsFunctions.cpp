#include <algorithm>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/scripts/ScriptError.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

template <>
void cast<bool>(Data& var)
{
	if(var.getTypeIndex() == DataType::Entity)//Convert entity to bool
		var = var.get<entityx::Entity>().valid();
	else if(var.getTypeIndex() == DataType::String)//Convert string to bool
		var = var.get<std::string>().size() > 0;
	else if(var.getTypeIndex() == DataType::Integer)//Convert int to bool
		var = static_cast<bool>(var.get<int>());
	else if(var.getTypeIndex() == DataType::Float)//Convert float to bool
		var = static_cast<bool>(var.get<float>());
}

template <>
void cast<int>(Data& var)
{
	if(var.getTypeIndex() == DataType::Entity)//Convert entity to int
		throw ScriptError("Conversion from entity to integer is not allowed.");
	else if(var.getTypeIndex() == DataType::String)//Convert string to int
		var = std::stoi(var.get<std::string>());
	else if(var.getTypeIndex() == DataType::Boolean)//Convert bool to int
		var = static_cast<int>(var.get<bool>());
	else if(var.getTypeIndex() == DataType::Float)//Convert float to int
		var = static_cast<int>(var.get<float>());
}

template <>
void cast<float>(Data& var)
{
	if(var.getTypeIndex() == DataType::Entity)//Convert entity to float
		throw ScriptError("Conversion from entity to floating point is not allowed.");
	else if(var.getTypeIndex() == DataType::String)//Convert string to float
		var = std::stof(var.get<std::string>());
	else if(var.getTypeIndex() == DataType::Boolean)//Convert bool to float
		var = static_cast<float>(var.get<bool>());
	else if(var.getTypeIndex() == DataType::Integer)//Convert int to float
		var = static_cast<float>(var.get<int>());
}

template <>
void cast<std::string>(Data& var)
{
	if(var.getTypeIndex() == DataType::Integer)//Convert int to string
		var = std::to_string(var.get<int>());
	else if(var.getTypeIndex() == DataType::Boolean)//Convert bool to string
	{
		if(var.get<bool>())
			var = std::string("true");
		else
			var = std::string("false");
	}
	else if(var.getTypeIndex() == DataType::Float)//Convert float to string
		var = std::to_string(var.get<float>());
	else if(var.getTypeIndex() == DataType::Entity)//Convert entity to string
		throw ScriptError("Conversion from entity to string is not allowed.");
}

template <>
void cast<entityx::Entity>(Data& var)
{
	if(var.getTypeIndex() != DataType::Entity)
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
	if(lhs.getTypeIndex() != DataType::String
			and rhs.getTypeIndex() != DataType::String
			and lhs.getTypeIndex() != DataType::Entity
			and rhs.getTypeIndex() != DataType::Entity)
	{
		if(lhs.getTypeIndex() != DataType::Float and rhs.getTypeIndex() != DataType::Float)
		{
			cast<int>(lhs);
			cast<int>(rhs);
			return lhs.get<int>() < rhs.get<int>();
		}
		else
		{
			cast<float>(lhs);
			cast<float>(rhs);
			return lhs.get<float>() < rhs.get<float>();
		}
	}
	else if(lhs.getTypeIndex() == DataType::String and rhs.getTypeIndex() == DataType::String)
		return lhs.get<std::string>().size() < rhs.get<std::string>().size();
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
	if(lhs.getTypeIndex() == DataType::String and rhs.getTypeIndex() == DataType::String)
		return lhs.get<std::string>() == rhs.get<std::string>();
	//Left is entity, right is bool
	else if(lhs.getTypeIndex() == DataType::Entity and rhs.getTypeIndex() == DataType::Boolean)
		return lhs.get<entityx::Entity>().valid() == rhs.get<bool>();
	//Left is bool, right is entity
	else if(lhs.getTypeIndex() == DataType::Boolean and rhs.getTypeIndex() == DataType::Entity)
		return lhs.get<bool>() == rhs.get<entityx::Entity>().valid();
	//Both are numeric
	else if(lhs.getTypeIndex() <= DataType::Float and rhs.getTypeIndex() <= DataType::Float)
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
	return lhs.get<bool>() and rhs.get<bool>();
}

Data orOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	cast<bool>(lhs);
	cast<bool>(rhs);
	return lhs.get<bool>() or rhs.get<bool>();
}

Data addOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() == DataType::Entity or rhs.getTypeIndex() == DataType::Entity)
		throw ScriptError("entities does not support addition.");
	else if(lhs.getTypeIndex() == DataType::String or rhs.getTypeIndex() == DataType::String)
	{
		cast<std::string>(lhs);
		cast<std::string>(rhs);
		return lhs.get<std::string>() + rhs.get<std::string>();
	}
	else if(lhs.getTypeIndex() == DataType::Float or rhs.getTypeIndex() == DataType::Float)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return lhs.get<float>() + rhs.get<float>();
	}
	else if(lhs.getTypeIndex() == DataType::Integer or rhs.getTypeIndex() == DataType::Integer)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return lhs.get<int>() + rhs.get<int>();
	}
	else
		return static_cast<bool>(lhs.get<bool>() + rhs.get<bool>());
}

Data substractOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() == DataType::Entity or rhs.getTypeIndex() == DataType::Entity)
		throw ScriptError("entities does not support substraction.");
	else if(lhs.getTypeIndex() == DataType::String or rhs.getTypeIndex() == DataType::String)
		throw std::runtime_error("string does not support substraction");
	else if(lhs.getTypeIndex() == DataType::Float or rhs.getTypeIndex() == DataType::Float)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return lhs.get<float>() - rhs.get<float>();
	}
	else if(lhs.getTypeIndex() == DataType::Integer or rhs.getTypeIndex() == DataType::Integer)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return lhs.get<int>() - rhs.get<int>();
	}
	else
		return static_cast<bool>(lhs.get<bool>() + rhs.get<bool>());
}

Data multiplyOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() == DataType::Entity or rhs.getTypeIndex() == DataType::Entity)
		throw ScriptError("entities does not support multiplication.");
	else if(lhs.getTypeIndex() == DataType::String or rhs.getTypeIndex() == DataType::String)
		throw std::runtime_error("string does not support multiplication.");
	else if(lhs.getTypeIndex() == DataType::Float or rhs.getTypeIndex() == DataType::Float)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		return lhs.get<float>() * rhs.get<float>();
	}
	else if(lhs.getTypeIndex() == DataType::Integer or rhs.getTypeIndex() == DataType::Integer)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		return lhs.get<int>() * rhs.get<int>();
	}
	else
		return static_cast<bool>(lhs.get<bool>() * rhs.get<bool>());
}

Data divideOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() == DataType::Entity or rhs.getTypeIndex() == DataType::Entity)
		throw ScriptError("entities does not support division.");
	else if(lhs.getTypeIndex() == DataType::String or rhs.getTypeIndex() == DataType::String)
		throw std::runtime_error("string does not support division.");
	else if(lhs.getTypeIndex() == DataType::Float or rhs.getTypeIndex() == DataType::Float)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		if(rhs.get<float>() == 0.f)
			throw std::runtime_error("division by zero");
		return lhs.get<float>() / rhs.get<float>();
	}
	else if(lhs.getTypeIndex() == DataType::Integer or rhs.getTypeIndex() == DataType::Integer)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		if(rhs.get<int>() == 0)
			throw std::runtime_error("division by zero");
		return lhs.get<int>() / rhs.get<int>();
	}
	else
		throw std::runtime_error("boolean does not support division.");
}

Data moduloOp(const std::vector<Data>& args)
{
	Data lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() == DataType::Entity or rhs.getTypeIndex() == DataType::Entity)
		throw ScriptError("entities does not support modulo.");
	else if(lhs.getTypeIndex() == DataType::String or rhs.getTypeIndex() == DataType::String)
		throw std::runtime_error("string does not support modulo.");
	else if(lhs.getTypeIndex() == DataType::Float or rhs.getTypeIndex() == DataType::Float)
	{
		cast<float>(lhs);
		cast<float>(rhs);
		if(rhs.get<float>() == 0.f)
			throw std::runtime_error("modulo by zero");
		return std::fmod(lhs.get<float>(), rhs.get<float>());
	}
	else if(lhs.getTypeIndex() == DataType::Integer or rhs.getTypeIndex() == DataType::Integer)
	{
		cast<int>(lhs);
		cast<int>(rhs);
		if(rhs.get<int>() == 0)
			throw std::runtime_error("modulo by zero");
		return lhs.get<int>() % rhs.get<int>();
	}
	else
		throw std::runtime_error("boolean does not support modulo.");
}

Data notOp(const std::vector<Data>& args)
{
	Data lhs(args[0]);
	cast<bool>(lhs);
	return not lhs.get<bool>();
}

entityx::Entity nearestFoe(const std::vector<Data>& args)
{
	entityx::Entity self(args[0].get<entityx::Entity>());
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
	entityx::Entity self(args[0].get<entityx::Entity>());
	entityx::Entity target(args[1].get<entityx::Entity>());
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
	entityx::Entity self(args[0].get<entityx::Entity>());
	entityx::Entity target(args[1].get<entityx::Entity>());
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
	entityx::Entity self(args[0].get<entityx::Entity>());
	if(not self)
		throw ScriptError("direction of(): first argument is an invalid entity.");
	DirectionComponent::Handle directionComponent(self.component<DirectionComponent>());
	if(directionComponent)
		return static_cast<int>(directionComponent->direction);
	return static_cast<int>(Direction::None);
}

int attack(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not entity.valid())
		throw ScriptError("can move(): first argument is an invalid entity.");
	Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, HandToHand());
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
	entityx::Entity self(args[0].get<entityx::Entity>());
	if(not self)
		throw ScriptError("can move(): first argument is an invalid entity.");
	return self.has_component<WalkComponent>();
}

int move(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not entity.valid())
		throw ScriptError("can move(): first argument is an invalid entity.");
	Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(static_cast<Direction>(args[1].get<int>())));
	return 0;
}

int stop(const std::vector<Data>& args)
{
	entityx::Entity self(args[0].get<entityx::Entity>());
	if(not self)
		throw ScriptError("stop(): first argument is an invalid entity.");
	const DirectionComponent::Handle directionComponent(self.component<DirectionComponent>());
	const WalkComponent::Handle walkComponent(self.component<WalkComponent>());
	if(directionComponent and walkComponent)
	{
		if(directionComponent->moveToLeft and directionComponent->moveToRight)
		{
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(self, Mover(not directionComponent->direction, false));
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(self, Mover(directionComponent->direction, false));
		}
		else
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(self, Mover(directionComponent->direction, false));
	}
	return 0;
}

bool canJump(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not entity)
		throw ScriptError("can jump(): first argument is an invalid entity.");
	const FallComponent::Handle fallComponent(entity.component<FallComponent>());
	return entity.has_component<JumpComponent>() and fallComponent and not fallComponent->inAir;
}

int jump(const std::vector<Data>& args)
{
	if(canJump(args))
		//Simply push a jump command on the command queue
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(args[0].get<entityx::Entity>(), Jumper());
	return 0;
}
