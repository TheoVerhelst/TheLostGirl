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
		case DataType::Float:
		case DataType::Integer:
			return andOp({greaterEqualOp(args), lowerEqualOp(args)});
		default:
			throw std::runtime_error("unsupported operands types for comparaison.");
	}
}

Data notEqualOp(const std::vector<Data>& args)
{
	return notOp({equalOp(args)});
}

Data andOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	return lhs.get<bool>() and rhs.get<bool>();
}

Data orOp(const std::vector<Data>& args)
{
	const Data& lhs(args[0]), rhs(args[1]);
	return lhs.get<bool>() or rhs.get<bool>();
}

Data addOp(const std::vector<Data>& args)
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

Data substractOp(const std::vector<Data>& args)
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

Data multiplyOp(const std::vector<Data>& args)
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

Data divideOp(const std::vector<Data>& args)
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

Data moduloOp(const std::vector<Data>& args)
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

Data notOp(const std::vector<Data>& args)
{
	Data lhs(args[0]);
	return not lhs.get<bool>();
}

entityx::Entity nearestFoe(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("nearest foe(): first argument is an invalid entity.");
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	DetectionRangeComponent::Handle detectionRangeComponent(entity.component<DetectionRangeComponent>());
	if(bodyComponent and detectionRangeComponent)
	{
		const float range{detectionRangeComponent->detectionRange/Context::parameters->pixelByMeter};
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

float distanceFrom(const std::vector<Data>& args)
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

int directionTo(const std::vector<Data>& args)
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
			return static_cast<int>(Direction::Right);
		else
			return static_cast<int>(Direction::Left);
	}
	return static_cast<int>(Direction::None);
}

int directionOf(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("direction of(): first argument is an invalid entity.");
	DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(directionComponent)
		return static_cast<int>(directionComponent->direction);
	return static_cast<int>(Direction::None);
}

int attack(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
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

bool canMove(const std::vector<Data>& args)
{
	const entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can move(): first argument is an invalid entity.");
	return entity.has_component<WalkComponent>();
}

int move(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
		throw ScriptError("can move(): first argument is an invalid entity.");
	Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(static_cast<Direction>(args[1].get<int>())));
	return 0;
}

int stop(const std::vector<Data>& args)
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
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(not directionComponent->direction, false));
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(directionComponent->direction, false));
		}
		else
			Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(entity, Mover(directionComponent->direction, false));
	}
	return 0;
}

bool canJump(const std::vector<Data>& args)
{
	entityx::Entity entity(args[0].get<entityx::Entity>());
	if(not TEST(entity.valid()))
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

bool isValid(const std::vector<Data>& args)
{
	return args[0].get<entityx::Entity>().valid();
}
