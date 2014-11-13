#include <dist/json/json.h>
#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2Body.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>

#include <TheLostGirl/serialization.h>

Json::Value serialize(entityx::ComponentHandle<BodyComponent> component)
{
	Json::Value ret;
	for(auto& bodyPair : component->bodies)
	{
		const std::string partName{bodyPair.first};
		b2Body* body{bodyPair.second};
		switch(body->GetType())
		{
			case b2_kinematicBody:
				ret[partName]["body"]["type"] = "kinematic";
				break;
			case b2_dynamicBody:
				ret[partName]["body"]["type"] = "dynamic";
				break;
			case b2_staticBody:
			default:
				ret[partName]["body"]["type"] = "static";
				break;
		}
									
		ret[partName]["body"]["position"]["x"] = body->GetPosition().x;
		ret[partName]["body"]["position"]["y"] = body->GetPosition().y;
		ret[partName]["body"]["angle"] = body->GetAngle();
		ret[partName]["body"]["linear velocity"]["x"] = body->GetLinearVelocity().x;
		ret[partName]["body"]["linear velocity"]["y"] = body->GetLinearVelocity().y;
		ret[partName]["body"]["angular velocity"] = body->GetAngularVelocity();
		ret[partName]["body"]["linear damping"] = body->GetLinearDamping();
		ret[partName]["body"]["angular damping"] = body->GetAngularDamping();
		ret[partName]["body"]["allow sleep"] = body->IsSleepingAllowed();
		ret[partName]["body"]["awake"] = body->IsAwake();
		ret[partName]["body"]["fixed rotation"] = body->IsFixedRotation();
		ret[partName]["body"]["bullet"] = body->IsBullet();
		ret[partName]["body"]["active"] = body->IsActive();
		ret[partName]["body"]["gravity scale"] = body->GetGravityScale();
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager)
{
	Json::Value ret;
	for(auto& spritePair : component->sprites)
	{
		const std::string partName{spritePair.first};
		const sf::Texture* tex = spritePair.second.getTexture();
		//Compare the pointer to the texture
		if(tex == &textureManager.get("archer"))
			ret[partName]["sprite"]["identifier"] = "archer";
		else if(tex == &textureManager.get("arms"))
			ret[partName]["sprite"]["identifier"] = "arms";
		else if(tex == &textureManager.get("bow"))
			ret[partName]["sprite"]["identifier"] = "bow";
	}
	for(auto& positionPair : component->worldPositions)
	{
		const std::string partName{positionPair.first};
		ret[partName]["sprite"]["position"]["x"] = positionPair.second.x;
		ret[partName]["sprite"]["position"]["y"] = positionPair.second.y;
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<sf::Sprite>> component)
{
	Json::Value ret;
	for(auto& animationsPair : component->animationsManagers)
		ret[animationsPair.first] = animationsPair.second.serialize();
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