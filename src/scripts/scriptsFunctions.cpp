#include <algorithm>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/Command.hpp>
#include <TheLostGirl/actions.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/scripts/scriptsFunctions.hpp>
#include <TheLostGirl/scripts/ScriptError.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>

Data ScriptFunctions::print(const std::vector<Data>& args)
{
	std::cout << std::boolalpha;
	for(size_t i(0); i < args.size(); ++i)
		std::cout << args[i] << " ";
	std::cout << std::endl;
	return 0;
}

Data ScriptFunctions::lowerThanOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for comparaison.");
	else
	{
		if(lhs.getTypeIndex() == DataType::Float)
			return lhs.get<float>() < rhs.get<float>();
		else if(lhs.getTypeIndex() == DataType::Integer)
			return lhs.get<int>() < rhs.get<int>();
		else
			throw std::runtime_error("unsupported operands types for comparaison.");
	}
}

Data ScriptFunctions::greaterThanOp(const std::vector<Data>& args)
{
	return lowerThanOp({args[1], args[0]});
}

Data ScriptFunctions::lowerEqualOp(const std::vector<Data>& args)
{
	return notOp({greaterThanOp(args)});
}

Data ScriptFunctions::greaterEqualOp(const std::vector<Data>& args)
{
	return notOp({lowerThanOp(args)});
}

Data ScriptFunctions::equalOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for comparaison.");
	switch(lhs.getTypeIndex())
	{
		case DataType::String:
			return lhs.get<std::string>() == rhs.get<std::string>();
		case DataType::Entity:
			return lhs.get<entityx::Entity>() == rhs.get<entityx::Entity>();
		case DataType::Boolean:
			return lhs.get<bool>() == rhs.get<bool>();
		case DataType::Direction:
			return lhs.get<Direction>() == rhs.get<Direction>();
		case DataType::Float:
		case DataType::Integer:
			return andOp({greaterEqualOp(args), lowerEqualOp(args)});
		default:
			throw std::runtime_error("unsupported operands types for comparaison.");
	}
}

Data ScriptFunctions::notEqualOp(const std::vector<Data>& args)
{
	return notOp({equalOp(args)});
}

Data ScriptFunctions::andOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	return lhs.get<bool>() and rhs.get<bool>();
}

Data ScriptFunctions::orOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	return lhs.get<bool>() or rhs.get<bool>();
}

Data ScriptFunctions::addOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for addition.");
	switch(lhs.getTypeIndex())
	{
		case DataType::String:
			return lhs.get<std::string>() + rhs.get<std::string>();
		case DataType::Float:
			return lhs.get<float>() + rhs.get<float>();
		case DataType::Integer:
			return lhs.get<int>() + rhs.get<int>();
		default:
			throw std::runtime_error("unsupported operands types for addition.");
	}
}

Data ScriptFunctions::substractOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for substraction.");
	switch(lhs.getTypeIndex())
	{
		case DataType::Float:
			return lhs.get<float>() - rhs.get<float>();
		case DataType::Integer:
			return lhs.get<int>() - rhs.get<int>();
		default:
			throw std::runtime_error("unsupported operands types for substraction.");
	}
}

Data ScriptFunctions::multiplyOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for multiplication.");
	switch(lhs.getTypeIndex())
	{
		case DataType::Float:
			return lhs.get<float>() * rhs.get<float>();
		case DataType::Integer:
			return lhs.get<int>() * rhs.get<int>();
		default:
			throw std::runtime_error("unsupported operands types for multiplication.");
	}
}

Data ScriptFunctions::divideOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for division.");
	switch(lhs.getTypeIndex())
	{
		case DataType::Float:
			return lhs.get<float>() / rhs.get<float>();
		case DataType::Integer:
			if(rhs.get<int>() == 0)
				throw std::runtime_error("division by 0.");
			return lhs.get<int>() / rhs.get<int>();
		default:
			throw std::runtime_error("unsupported operands types for division.");
	}
}

Data ScriptFunctions::moduloOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	if(lhs.getTypeIndex() != rhs.getTypeIndex())
		throw std::runtime_error("different operands types for division.");
	switch(lhs.getTypeIndex())
	{
		case DataType::Float:
			if(rhs.get<float>() == 0.f)
				throw std::runtime_error("modulo by 0.");
			return std::fmod(lhs.get<float>(), rhs.get<float>());
		case DataType::Integer:
			if(rhs.get<int>() == 0)
				throw std::runtime_error("modulo by 0.");
			return lhs.get<int>() % rhs.get<int>();
		default:
			throw std::runtime_error("unsupported operands types for division.");
	}
}

Data ScriptFunctions::notOp(const std::vector<Data>& args)
{
	Data lhs(args[0]);
	return not lhs.get<bool>();
}

entityx::Entity ScriptFunctions::nearestFoe(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("nearest foe(): first argument is an invalid entity.");
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	DetectionRangeComponent::Handle detectionRangeComponent(entity.component<DetectionRangeComponent>());
	if(bodyComponent and detectionRangeComponent)
	{
		const float range{detectionRangeComponent->detectionRange/getParameters().pixelByMeter};
		b2Body* body{bodyComponent->body};
		b2World* world{body->GetWorld()};
		NearestFoeQueryCallback callback(entity);
		b2AABB aabb;
		aabb.lowerBound = b2Vec2(-range, -range) + body->GetWorldCenter();
		aabb.upperBound = b2Vec2(range, range) + body->GetWorldCenter();
		world->QueryAABB(&callback, aabb);
		return callback.entity;
	}
	return entityx::Entity();
}

float ScriptFunctions::distanceFrom(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	entityx::Entity target(args[1].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("distance from(): first argument is an invalid entity.");
	if(not TEST(target.valid()))
		throw ScriptError("distance from(): second argument is an invalid entity.");
	if(entity == target)
		return 0.f;
	BodyComponent::Handle selfBodyComponent(entity.component<BodyComponent>());
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

Direction ScriptFunctions::directionTo(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	entityx::Entity target(args[1].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("direction to(): first argument is an invalid entity.");
	if(not TEST(target.valid()))
		throw ScriptError("direction to(): second argument is an invalid entity.");
	TransformComponent::Handle selfTransformComponent(entity.component<TransformComponent>());
	TransformComponent::Handle targetTransformComponent(target.component<TransformComponent>());
	if(selfTransformComponent and targetTransformComponent)
	{
		if(selfTransformComponent->transform.x < targetTransformComponent->transform.x)
			return Direction::Right;
		else
			return Direction::Left;
	}
	return Direction::None;
}

Direction ScriptFunctions::directionOf(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("direction of(): first argument is an invalid entity.");
	DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(directionComponent)
		directionComponent->direction;
	return Direction::None;
}

int ScriptFunctions::attack(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can move(): first argument is an invalid entity.");
	getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, HandToHand());
	return 0;
}

ScriptFunctions::NearestFoeQueryCallback::NearestFoeQueryCallback(entityx::Entity self):
	entity(),
	m_self(self),
	m_distance(std::numeric_limits<float>::infinity())
{}

bool ScriptFunctions::NearestFoeQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity currentEntity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	if(not TEST(currentEntity.valid()))
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

bool ScriptFunctions::canMove(const std::vector<Data>& args)
{
	const entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can move(): first argument is an invalid entity.");
	return entity.has_component<WalkComponent>();
}

int ScriptFunctions::move(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can move(): first argument is an invalid entity.");
	getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(args[1].get<Direction>()));
	return 0;
}

int ScriptFunctions::stop(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("stop(): first argument is an invalid entity.");
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	const WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
	if(directionComponent and walkComponent)
	{
		if(directionComponent->moveToLeft and directionComponent->moveToRight)
		{
			getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(not directionComponent->direction, false));
			getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(directionComponent->direction, false));
		}
		else
			getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(directionComponent->direction, false));
	}
	return 0;
}

bool ScriptFunctions::canJump(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can jump(): first argument is an invalid entity.");
	const FallComponent::Handle fallComponent(entity.component<FallComponent>());
	return entity.has_component<JumpComponent>() and fallComponent and not fallComponent->inAir;
}

int ScriptFunctions::jump(const std::vector<Data>& args)
{
	if(canJump(args))
		//Simply push a jump command on the command queue
		getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(args[0].get<entityx::Entity>(), Jumper());
	return 0;
}

bool ScriptFunctions::isValid(const std::vector<Data>& args)
{
	return args[0].get<entityx::Entity>().valid();
}
