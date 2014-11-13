#include <dist/json/json.h>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/serialization.h>

Json::Value serialize(entityx::ComponentHandle<BodyComponent> component)
{
	Json::Value ret;
	for(auto& bodyPair : component->bodies)
	{
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component)
{
	Json::Value ret;
	for(auto& spritePair : component->sprites)
	{
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<sf::Sprite>> component)
{
	Json::Value ret;
	for(auto& animationsPair : component->animationsManagers)
	{
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<DirectionComponent> component)
{
	Json::Value ret;
	switch(component->direction)
	{
		case Direction::Left:
			ret["direction"] = "left";
			break;
		case Direction::Right:
			ret["direction"] = "right";
			break;
		case Direction::Top:
			ret["direction"] = "top";
			break;
		case Direction::Bottom:
			ret["direction"] = "bottom";
			break;
		default:
			ret["direction"] = "none";
			break;
	}
	ret["move to left"] = component->moveToLeft;
	ret["move to right"] = component->moveToRight;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<CategoryComponent> component)
{
	Json::Value ret;
	if(component->category & Category::Player)
		ret.append("player");
	if(component->category & Category::Scene)
		ret.append("scene");
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<ActorIDComponent> component)
{
	return component->ID;
}

Json::Value serialize(entityx::ComponentHandle<SkyComponent> component)
{
	return component->day;
}

Json::Value serialize(entityx::ComponentHandle<FallComponent> component)
{
	Json::Value ret;
	ret["in air"] = component->inAir;
	ret["contact count"] = component->contactCount;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<WalkComponent> component)
{
	Json::Value ret;
	switch(component->effectiveMovement)
	{
		case Direction::Left:
			ret["effective movement"] = "left";
			break;
		case Direction::Right:
			ret["effective movement"] = "right";
			break;
		case Direction::Top:
			ret["effective movement"] = "top";
			break;
		case Direction::Bottom:
			ret["effective movement"] = "bottom";
			break;
		default:
			ret["effective movement"] = "none";
			break;
	}
	ret["speed"] = component->walkSpeed;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<JumpComponent> component)
{
	Json::Value ret;
	ret["strength"] = component->jumpStrength;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<BendComponent> component)
{
	Json::Value ret;
	ret["power"] = component->power;
	ret["maximum power"] = component->maxPower;
	ret["angle"] = component->angle;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<HealthComponent> component)
{
	Json::Value ret;
	ret["current health"] = component->health;
	ret["maximum health"] = component->maxHealth;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<StaminaComponent> component)
{
	Json::Value ret;
	ret["current stamina"] = component->stamina;
	ret["maximum stamina"] = component->maxStamina;
	return ret;
}