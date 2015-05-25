#include <dist/json/json.h>
#include <entityx/Entity.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/b2World.h>
#include <SFML/Graphics/Texture.hpp>

#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/serialization.h>

  //////////             ///////////
 ////////// Serializing ///////////
//////////             ///////////

Json::Value serialize(entityx::ComponentHandle<BodyComponent> component, float pixelByMeter)
{
	Json::Value ret;
	for(auto& bodyPair : component->bodies)
	{
		const std::string partName{bodyPair.first};
		b2Body* body{bodyPair.second};
		switch(body->GetType())
		{
			case b2_kinematicBody:
				ret[partName]["type"] = "kinematic";
				break;
			case b2_dynamicBody:
				ret[partName]["type"] = "dynamic";
				break;
			case b2_staticBody:
			default:
				ret[partName]["type"] = "static";
				break;
		}

		ret[partName]["linear velocity"]["x"] = body->GetLinearVelocity().x*pixelByMeter;
		ret[partName]["linear velocity"]["y"] = body->GetLinearVelocity().y*pixelByMeter;
		ret[partName]["angular velocity"] = body->GetAngularVelocity();
		ret[partName]["linear damping"] = body->GetLinearDamping();
		ret[partName]["angular damping"] = body->GetAngularDamping();
		ret[partName]["allow sleep"] = body->IsSleepingAllowed();
		ret[partName]["awake"] = body->IsAwake();
		ret[partName]["fixed rotation"] = body->IsFixedRotation();
		ret[partName]["bullet"] = body->IsBullet();
		ret[partName]["active"] = body->IsActive();
		ret[partName]["gravity scale"] = body->GetGravityScale();

		//For each fixture of the body
		for(b2Fixture* fix{body->GetFixtureList()}; fix; fix = fix->GetNext())
		{
			Json::Value fixtureObj;
			b2Shape::Type type{fix->GetType()};
			switch(type)
			{
				case b2Shape::e_circle:
				{
					b2CircleShape* shape{static_cast<b2CircleShape*>(fix->GetShape())};
					fixtureObj["position"]["x"] = shape->m_p.x*pixelByMeter;
					fixtureObj["position"]["y"] = shape->m_p.y*pixelByMeter;
					fixtureObj["radius"] = shape->m_radius*pixelByMeter;
					break;
				}
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape{static_cast<b2EdgeShape*>(fix->GetShape())};
					//Copy all vertices
					if(shape->m_hasVertex0)
					{
						fixtureObj["0"]["x"] = shape->m_vertex0.x*pixelByMeter;
						fixtureObj["0"]["y"] = shape->m_vertex0.y*pixelByMeter;
					}
					fixtureObj["1"]["x"] = shape->m_vertex1.x*pixelByMeter;
					fixtureObj["1"]["y"] = shape->m_vertex1.y*pixelByMeter;
					fixtureObj["2"]["x"] = shape->m_vertex2.x*pixelByMeter;
					fixtureObj["2"]["y"] = shape->m_vertex2.y*pixelByMeter;
					if(shape->m_hasVertex3)
					{
						fixtureObj["3"]["x"] = shape->m_vertex3.x*pixelByMeter;
						fixtureObj["3"]["y"] = shape->m_vertex3.y*pixelByMeter;
					}
					break;
				}
				case b2Shape::e_polygon:
				{
					b2PolygonShape* shape{static_cast<b2PolygonShape*>(fix->GetShape())};
					//Copy all vertices
					for(int32 j{0}; j < shape->GetVertexCount(); ++j)
					{
						fixtureObj["vertices"][j]["x"] = shape->m_vertices[j].x*pixelByMeter;
						fixtureObj["vertices"][j]["y"] = shape->m_vertices[j].y*pixelByMeter;
					}
					break;
				}
				default:
				case b2Shape::e_chain:
				{
					b2ChainShape* shape{static_cast<b2ChainShape*>(fix->GetShape())};
					//Copy all vertices
					if(shape->m_hasPrevVertex)
					{
						fixtureObj["previous vertex"]["x"] = shape->m_prevVertex.x*pixelByMeter;
						fixtureObj["previous vertex"]["y"] = shape->m_prevVertex.y*pixelByMeter;
					}
					for(int32 j{0}; j < shape->m_count; ++j)
					{
						fixtureObj["vertices"][j]["x"] = shape->m_vertices[j].x*pixelByMeter;
						fixtureObj["vertices"][j]["y"] = shape->m_vertices[j].y*pixelByMeter;
					}
					if(shape->m_hasNextVertex)
					{
						fixtureObj["next vertex"]["x"] = shape->m_nextVertex.x*pixelByMeter;
						fixtureObj["next vertex"]["y"] = shape->m_nextVertex.y*pixelByMeter;
					}
					break;
				}
			}
			fixtureObj["density"] = fix->GetDensity();
			fixtureObj["friction"] = fix->GetFriction();
			fixtureObj["restitution"] = fix->GetRestitution();
			fixtureObj["is sensor"] = fix->IsSensor();
			//If the fixture is a foot sensor
			if(fixtureHasRole(fix, FixtureRole::Foot))
				fixtureObj["roles"].append("foot sensor");
			if(fixtureHasRole(fix, FixtureRole::Main))
				fixtureObj["roles"].append("main");
			//Add the fixture in the right array
			switch(type)
			{
				case b2Shape::e_circle:
					ret[partName]["circle fixtures"].append(fixtureObj);
					break;

				case b2Shape::e_edge:
					ret[partName]["edge fixtures"].append(fixtureObj);
					break;

				case b2Shape::e_polygon:
					ret[partName]["polygon fixtures"].append(fixtureObj);
					break;

				case b2Shape::e_chain:
					ret[partName]["chain fixtures"].append(fixtureObj);
					break;

				case b2Shape::e_typeCount:
				default:
					throw std::runtime_error("Invalid shape type.");
			}
		}
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, float scale)
{
	Json::Value ret;
	for(auto& spritePair : component->sprites)
	{
		const std::string partName{spritePair.first};//Get the name of the entity's part
		const sf::Texture* tex{spritePair.second.getTexture()};//Get the associated texture
		ret[partName]["identifier"] = textureManager.getIdentifier(*tex);//Get the identifier of the texture and put it in Json
		if(spritePair.second.getOrigin() != sf::Vector2f(0, 0))
		{
			ret[partName]["origin"]["x"] = spritePair.second.getOrigin().x/scale;
			ret[partName]["origin"]["y"] = spritePair.second.getOrigin().y/scale;
		}
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<TransformComponent> component)
{
	Json::Value ret;
	for(auto& transformPair : component->transforms)
	{
		const std::string partName{transformPair.first};
		ret[partName]["transform"]["x"] = transformPair.second.x;
		ret[partName]["transform"]["y"] = transformPair.second.y;
		ret[partName]["transform"]["z"] = transformPair.second.z;
		ret[partName]["transform"]["angle"] = transformPair.second.angle;
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<InventoryComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap)
{
	Json::Value ret;
	ret["maximum weight"] = component->maxWeight;
	ret["items"] = Json::Value(Json::arrayValue);
	//For each item of the inventory
	for(entityx::Entity item : component->items)
		//If the item is in the entity list
		if(isMember(entitiesMap, item) and item.valid())
			//Add it to the value
			ret["items"].append(getKey(entitiesMap, item));
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component)
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
		case Direction::None:
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
	if(component->category & Category::Passive)
		ret.append("passive");
	if(component->category & Category::Aggressive)
		ret.append("aggressive");
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<FallComponent> component)
{
	Json::Value ret;
	ret["falling resistance"] = component->fallingResistance;
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
		case Direction::None:
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

Json::Value serialize(entityx::ComponentHandle<BowComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap)
{
	Json::Value ret;
	ret["power"] = component->power;
	ret["maximum power"] = component->maxPower;
	ret["angle"] = component->angle;
	ret["quiver capacity"] = component->quiverCapacity;
	if(isMember(entitiesMap, component->notchedArrow) and component->notchedArrow.valid())
		ret["notched arrow"] = getKey(entitiesMap, component->notchedArrow);
	ret["arrows"] = Json::Value(Json::arrayValue);
	//For each arrow of the quiver
	for(entityx::Entity arrow : component->arrows)
		//If the arrow is in the entity list
		if(isMember(entitiesMap, arrow) and arrow.valid())
			//Add it to the value
			ret["arrows"].append(getKey(entitiesMap, arrow));
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<HealthComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<StaminaComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<ArrowComponent> component)
{
	Json::Value ret;
	ret["friction"] = component->friction;
	ret["penetrance"] = component->penetrance;
	ret["damage"] = component->damage;
	ret["local friction point"]["x"] = component->localFrictionPoint.x;
	ret["local friction point"]["y"] = component->localFrictionPoint.y;
	ret["local stick point"]["x"] = component->localStickPoint.x;
	ret["local stick point"]["y"] = component->localStickPoint.y;
	switch(component->state)
	{
		case ArrowComponent::Fired:
			ret["state"] = "fired";
			break;

		case ArrowComponent::Sticked:
			ret["state"] = "sticked";
			break;

		case ArrowComponent::Notched:
			ret["state"] = "notched";
			break;

		case ArrowComponent::Stored:
		default:
			ret["state"] = "stored";
			break;
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<HardnessComponent> component)
{
	return component->hardness;
}

Json::Value serialize(entityx::ComponentHandle<ScriptsComponent> component)
{
	Json::Value ret;
	ret["scripts"] = Json::Value(Json::arrayValue);
	for(auto& scriptName : component->scriptsNames)
        ret["scripts"].append(scriptName);
    return ret;
}

Json::Value serialize(entityx::ComponentHandle<DetectionRangeComponent> component)
{
	return component->detectionRange;
}

Json::Value serialize(entityx::ComponentHandle<DeathComponent> component)
{
	Json::Value ret;
	ret["dead"] = component->dead;
	ret["drops"] = Json::arrayValue;
	for(auto& drop : component->drops)
	{
		Json::Value dropJson;
		dropJson["category"] = drop.category;
		dropJson["type"] = drop.type;
		dropJson["probability"] = drop.probability;
		dropJson["max drops"] = drop.maxDrops;
		ret["drops"].append(dropJson);
	}
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<NameComponent> component)
{
	return component->name;
}

Json::Value serialize(entityx::ComponentHandle<HandToHandComponent> component)
{
	Json::Value ret;
	ret["damages"] = component->damages;
	ret["delay"] = component->delay.asSeconds();
	ret["last shoot"] = component->lastShoot.asSeconds();
	return ret;
}

Json::Value serialize(entityx::ComponentHandle<ActorComponent> component)
{
	Json::Value ret;
	ret["hand to hand resistance"] = component->handToHandResistance;
	ret["magic resistance"] = component->magicResistance;
	ret["arrow resistance"] = component->arrowResistance;
	ret["fire resistance"] = component->fireResistance;
	ret["frost resistance"] = component->frostResistance;
	ret["poison resistance"] = component->poisonResistance;
	return ret;
}

  //////////               //////////
 ////////// Deserializing //////////
//////////               //////////

void deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::ComponentHandle<TransformComponent> transformComponent, b2World& world, float pixelByMeter)
{
	component->bodies.clear();
	for(std::string& partName : value.getMemberNames())
	{
		if(transformComponent->transforms.find(partName) == transformComponent->transforms.end())
			throw std::runtime_error(partName + " part is defined in body component but not in transorms component.");
		else
		{
			Json::Value body{value[partName]};
			b2BodyDef entityBodyComponentDef;

			//type
			const Json::Value type{body["type"]};
			if(type == "static")
				entityBodyComponentDef.type = b2_staticBody;
			else if(type == "kinematic")
				entityBodyComponentDef.type = b2_kinematicBody;
			else if(type == "dynamic")
				entityBodyComponentDef.type = b2_dynamicBody;

			//position
			entityBodyComponentDef.position.x = transformComponent->transforms[partName].x/pixelByMeter;
			entityBodyComponentDef.position.y = transformComponent->transforms[partName].y/pixelByMeter;

			//angle
			entityBodyComponentDef.angle = transformComponent->transforms[partName].angle*b2_pi/180;
			const Json::Value linearVelocity{body["linear velocity"]};
			entityBodyComponentDef.linearVelocity.x = linearVelocity["x"].asFloat()/pixelByMeter;
			entityBodyComponentDef.linearVelocity.y = linearVelocity["y"].asFloat()/pixelByMeter;
			entityBodyComponentDef.angularVelocity = body["angular velocity"].asFloat();
			entityBodyComponentDef.linearDamping = body["linear damping"].asFloat();
			entityBodyComponentDef.angularDamping = body["angular damping"].asFloat();
			entityBodyComponentDef.allowSleep = body["allow sleep"].asBool();
			entityBodyComponentDef.awake = body["awake"].asBool();
			entityBodyComponentDef.fixedRotation = body["fixed rotation"].asBool();
			entityBodyComponentDef.bullet = body["bullet"].asBool();
			entityBodyComponentDef.active = body["active"].asBool();
			entityBodyComponentDef.gravityScale = body["gravity scale"].asFloat();

			b2Body* entityBodyComponent{world.CreateBody(&entityBodyComponentDef)};
			component->bodies.emplace(partName, entityBodyComponent);

			//polygon fixtures
			if(body.isMember("polygon fixtures"))
			{
				const Json::Value fixtures{body["polygon fixtures"]};
				for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
				{
					const Json::Value fixture{fixtures[i]};
					b2FixtureDef entityFixtureDef;
					b2PolygonShape polygonShape;

					//vertices
					const Json::Value vertices{fixtures[i]["vertices"]};
					std::vector<b2Vec2> verticesVec(vertices.size());
					for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
					{
						const Json::Value vertice{vertices[j]};
						verticesVec[j].x = vertice["x"].asFloat()/pixelByMeter;
						verticesVec[j].y = vertice["y"].asFloat()/pixelByMeter;
					}
					polygonShape.Set(verticesVec.data(), static_cast<int32>(verticesVec.size()));
					entityFixtureDef.shape = &polygonShape;

					entityFixtureDef.density = fixture["density"].asFloat();
					entityFixtureDef.friction = fixture["friction"].asFloat();
					entityFixtureDef.restitution = fixture["restitution"].asFloat();
					entityFixtureDef.isSensor = fixture["is sensor"].asBool();
					const Json::Value roles{fixture["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
					{
						if(roles[j].asString() == "foot sensor")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
						else if(roles[j].asString() == "main")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
					}
					entityBodyComponent->CreateFixture(&entityFixtureDef);
				}
			}

			//edge fixtures
			if(body.isMember("edge fixtures"))
			{
				const Json::Value fixtures{body["edge fixtures"]};
				for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
				{
					const Json::Value fixture{fixtures[i]};

					b2FixtureDef entityFixtureDef;
					b2EdgeShape edgeShape;
					b2Vec2 one{0, 0}, two{0, 0};

					one.x = fixtures[i]["1"]["x"].asFloat();
					one.y = fixtures[i]["1"]["y"].asFloat();
					two.x = fixtures[i]["2"]["x"].asFloat();
					two.y = fixtures[i]["2"]["y"].asFloat();

					edgeShape.Set((1.f/pixelByMeter)*one, (1.f/pixelByMeter)*two);

					//0
					if(fixture.isMember("0"))
					{
						edgeShape.m_hasVertex0 = true;
						edgeShape.m_vertex0.x = fixtures[i]["0"]["x"].asFloat()/pixelByMeter;
						edgeShape.m_vertex0.y = fixtures[i]["0"]["y"].asFloat()/pixelByMeter;
					}

					//3
					if(fixture.isMember("3"))
					{
						edgeShape.m_hasVertex3 = true;
						edgeShape.m_vertex3.x = fixtures[i]["3"]["x"].asFloat()/pixelByMeter;
						edgeShape.m_vertex3.y = fixtures[i]["3"]["y"].asFloat()/pixelByMeter;
					}
					entityFixtureDef.shape = &edgeShape;
					entityFixtureDef.density = fixture["density"].asFloat();
					entityFixtureDef.friction = fixture["friction"].asFloat();
					entityFixtureDef.restitution = fixture["restitution"].asFloat();
					entityFixtureDef.isSensor = fixture["is sensor"].asBool();

					//roles
					const Json::Value roles{body["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
					{
						if(roles[j].asString() == "foot sensor")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
						else if(roles[j].asString() == "main")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
					}

					entityBodyComponent->CreateFixture(&entityFixtureDef);
				}
			}

			//chain fixtures
			if(body.isMember("chain fixtures"))
			{
				const Json::Value fixtures{body["chain fixtures"]};
				for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
				{
					const Json::Value fixture{fixtures[i]};
					b2FixtureDef entityFixtureDef;
					b2ChainShape chainShape;
					std::vector<b2Vec2> verticesArray;
					const Json::Value vertices{fixtures[i]["vertices"]};
					//For each vertex of the chain shape
					for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
					{
						const Json::Value vertice{vertices[j]};
						verticesArray.push_back(b2Vec2(0, 0));
						verticesArray[j].x = vertice["x"].asFloat()/pixelByMeter;
						verticesArray[j].y = vertice["y"].asFloat()/pixelByMeter;
					}
					chainShape.CreateChain(verticesArray.data(), static_cast<int32>(verticesArray.size()));

					if(fixtures[i].isMember("previous vertex"))
					{
						chainShape.m_hasPrevVertex = true;
						chainShape.m_prevVertex.x = fixtures[i]["previous vertex"]["x"].asFloat()/pixelByMeter;
						chainShape.m_prevVertex.y = fixtures[i]["previous vertex"]["y"].asFloat()/pixelByMeter;
					}

					if(fixtures[i].isMember("next vertex"))
					{
						chainShape.m_hasNextVertex = true;
						chainShape.m_nextVertex.x = fixtures[i]["next vertex"]["x"].asFloat()/pixelByMeter;
						chainShape.m_nextVertex.y = fixtures[i]["next vertex"]["y"].asFloat()/pixelByMeter;
					}
					entityFixtureDef.shape = &chainShape;
					entityFixtureDef.density = fixture["density"].asFloat();
					entityFixtureDef.friction = fixture["friction"].asFloat();
					entityFixtureDef.restitution = fixture["restitution"].asFloat();
					entityFixtureDef.isSensor = fixture["is sensor"].asBool();
					const Json::Value roles{body["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
					{
						if(roles[j].asString() == "foot sensor")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
						else if(roles[j].asString() == "main")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
					}

					entityBodyComponent->CreateFixture(&entityFixtureDef);
				}
			}

			//circle fixtures
			if(body.isMember("circle fixtures"))
			{
				const Json::Value fixtures{body["circle fixtures"]};
				for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
				{
					const Json::Value fixture{fixtures[i]};

					b2FixtureDef entityFixtureDef;

					b2CircleShape circleShape;
					circleShape.m_p.x = fixtures[i]["position"]["x"].asFloat()/pixelByMeter;
					circleShape.m_p.y = fixtures[i]["position"]["y"].asFloat()/pixelByMeter;

					entityFixtureDef.shape = &circleShape;
					entityFixtureDef.density = fixture["density"].asFloat();
					entityFixtureDef.friction = fixture["friction"].asFloat();
					entityFixtureDef.restitution = fixture["restitution"].asFloat();
					entityFixtureDef.isSensor = fixture["is sensor"].asBool();
					const Json::Value roles{body["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
					{
						if(roles[j].asString() == "foot sensor")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
						else if(roles[j].asString() == "main")
							//Add the role to the data
							entityFixtureDef.userData = add(entityFixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
					}
					entityBodyComponent->CreateFixture(&entityFixtureDef);
				}
			}
		}
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, const std::string& path, float scale)
{
	component->sprites.clear();
	for(std::string& partName : value.getMemberNames())
	{
		const std::string identifier{value[partName]["identifier"].asString()};
		textureManager.load(identifier, path + "/" + identifier);
		component->sprites.emplace(partName, sf::Sprite(textureManager.get(identifier)));
		if(value[partName].isMember("origin"))
			component->sprites[partName].setOrigin(value[partName]["origin"]["x"].asFloat()*scale, value[partName]["origin"]["y"].asFloat()*scale);
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component)
{
	component->transforms.clear();
	for(std::string& partName : value.getMemberNames())
	{
		Json::Value part{value[partName]};
		Transform transform;
		transform.x = part["transform"]["x"].asFloat();
		transform.y = part["transform"]["y"].asFloat();
		transform.z = part["transform"]["z"].asFloat();
		transform.angle = part["transform"]["angle"].asFloat();
		component->transforms.emplace(partName, transform);
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap)
{
	component->items.clear();
	for(Json::ArrayIndex i{0}; i < value["items"].size(); ++i)
		if(entitiesMap.find(value["items"][i].asString()) != entitiesMap.end())
			component->items.push_back(entitiesMap.at(value["items"][i].asString()));
	component->maxWeight = value["maximum weight"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent, StateStack::Context context)
{
	component->animationsManagers.clear();
	for(std::string& partName : value.getMemberNames())
	{
		//If the associated sprite exists
		if(spriteComponent->sprites.find(partName) == spriteComponent->sprites.end())
			throw std::runtime_error(partName + " part is defined in spritesheet animations component but not in sprite component.");
		else
		{
			AnimationsManager<SpriteSheetAnimation> animationsManager;
			animationsManager.deserialize(value[partName], spriteComponent->sprites[partName], context);
			component->animationsManagers.emplace(partName, animationsManager);
		}
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component)
{
	if(value["direction"] == "left")
		component->direction = Direction::Left;
	else if(value["direction"] == "right")
		component->direction = Direction::Right;
	else if(value["direction"] == "top")
		component->direction = Direction::Top;
	else if(value["direction"] == "bottom")
		component->direction = Direction::Bottom;
	if(value.isMember("move to left"))
		component->moveToLeft = value["move to left"].asBool();
	if(value.isMember("move to right"))
		component->moveToRight = value["move to right"].asBool();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component)
{
	unsigned int category{0};
	for(Json::ArrayIndex i{0}; i < value.size(); ++i)
	{
		if(value[i] == "player")
			category |= Category::Player;
		else if(value[i] == "scene")
			category |= Category::Scene;
		else if(value[i] == "passive")
			category |= Category::Passive;
		else if(value[i] == "aggressive")
			category |= Category::Aggressive;
	}
	component->category = category;
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component)
{
	component->fallingResistance = value["falling resistance"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component)
{
	if(value.isMember("effective movement"))
	{
		if(value["effective movement"] == "left")
			component->effectiveMovement = Direction::Left;
		else if(value["effective movement"] == "right")
			component->effectiveMovement = Direction::Right;
		else if(value["effective movement"] == "top")
			component->effectiveMovement = Direction::Top;
		else if(value["effective movement"] == "bottom")
			component->effectiveMovement = Direction::Bottom;
		else
			component->effectiveMovement = Direction::None;
	}
	component->walkSpeed = value["speed"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component)
{
	component->jumpStrength = value["strength"].asFloat();
	component->mustJump = false;
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, const std::map<std::string, entityx::Entity>& entitiesMap)
{
	component->power = value["power"].asFloat();
	component->angle = value["angle"].asFloat();
	component->maxPower = value["maximum power"].asFloat();
	component->arrows.clear();
	for(Json::ArrayIndex i{0}; i < value["arrows"].size(); ++i)
		if(entitiesMap.find(value["arrows"][i].asString()) != entitiesMap.end())
			component->arrows.push_back(entitiesMap.at(value["arrows"][i].asString()));
	if(entitiesMap.find(value["notched arrow"].asString()) != entitiesMap.end())
		component->notchedArrow = entitiesMap.at(value["notched arrow"].asString());
	component->quiverCapacity = value["quiver capacity"].asUInt();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<ArrowComponent> component)
{
	component->friction = value["friction"].asFloat();
	component->penetrance = value["penetrance"].asFloat();
	component->damage = value["damage"].asFloat();
	component->localFrictionPoint.x = value["local friction point"]["x"].asFloat();
	component->localFrictionPoint.y = value["local friction point"]["y"].asFloat();
	component->localStickPoint.x = value["local stick point"]["x"].asFloat();
	component->localStickPoint.y = value["local stick point"]["y"].asFloat();
	if(value["state"] == "fired")
		component->state = ArrowComponent::Fired;
	else if(value["state"] == "sticked")
		component->state = ArrowComponent::Sticked;
	else if(value["state"] == "stored")
		component->state = ArrowComponent::Stored;
	else if(value["state"] == "notched")
		component->state = ArrowComponent::Notched;
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<HardnessComponent> component)
{
	component->hardness = value.asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component, ScriptManager& scriptManager)
{
	for(Json::ArrayIndex i{0}; i < value["scripts"].size(); ++i)
	{
		std::string scriptName{value["scripts"][i].asString()};
		scriptManager.load(scriptName, "resources/scripts/" + scriptName);
        component->scriptsNames.push_back(scriptName);
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<DetectionRangeComponent> component)
{
	component->detectionRange = value.asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<DeathComponent> component)
{
	component->dead = value["dead"].asBool();
	for(Json::ArrayIndex i{0}; i < value["drops"].size(); ++i)
		component->drops.push_back({value["drops"][i]["category"].asString(),
									value["drops"][i]["type"].asString(),
									value["drops"][i]["probability"].asFloat(),
									value["drops"][i]["max drops"].asUInt()});
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<NameComponent> component)
{
	component->name = value.asString();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<HandToHandComponent> component)
{
	component->damages = value["damages"].asFloat();
	component->delay = sf::seconds(value["delay"].asFloat());
	component->lastShoot = sf::seconds(value["last shoot"].asFloat());
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<ActorComponent> component)
{
	component->handToHandResistance = value["hand to hand resistance"].asFloat();
	component->magicResistance = value["magic resistance"].asFloat();
	component->arrowResistance = value["arrow resistance"].asFloat();
	component->fireResistance = value["fire resistance"].asFloat();
	component->frostResistance = value["frost resistance"].asFloat();
	component->poisonResistance = value["poison resistance"].asFloat();
}

std::string typeToStr(Json::ValueType type)
{
	//The "n " or " " liaise the words
	switch(type)
	{
		case Json::booleanValue:
			return " boolean number";
		case Json::intValue:
			return "n integer number";
		case Json::uintValue:
			return "n unsigned integer number";
		case Json::realValue:
			return " real number";
		case Json::stringValue:
			return " string";
		case Json::arrayValue:
			return "n array";
		case Json::objectValue:
			return "n object";
		case Json::nullValue:
		default:
			return " null value";
	}
}

Json::ValueType strToType(std::string str)
{
	if(str == "string")
		return Json::stringValue;
	else if(str == "object")
		return Json::objectValue;
	else if(str == "array")
		return Json::arrayValue;
	else if(str == "real")
		return Json::realValue;
	else if(str == "int")
		return Json::intValue;
	else if(str == "uint")
		return Json::uintValue;
	else if(str == "bool")
		return Json::booleanValue;
	else
		return Json::nullValue;
}

void parseObject(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(std::string elementName : object.getMemberNames())
		{
			if(valuesTypes.find(elementName) == valuesTypes.end())//If the value in the object does not exists in the map
				throw std::runtime_error("\"" + name + "." + elementName + "\" identifier is not recognized.");
			else if(not object[elementName].isConvertibleTo(valuesTypes[elementName]))//If the value exists but have not the right type
				throw std::runtime_error("\"" + name + "." + elementName + "\" must be a" + typeToStr(valuesTypes[elementName]) + ".");
		}
	}
}

void requireValues(const Json::Value& object, const std::string name, std::map<std::string, Json::ValueType> valuesTypes)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(auto& pair : valuesTypes)
		{
			if(not object.isMember(pair.first))//If the value in the map does not exists in the object
				throw std::runtime_error("\"" + name + "." + pair.first + "\" value must be defined.");
			else if(not object[pair.first].isConvertibleTo(valuesTypes[pair.first]))//If the value exists but have not the right type
				throw std::runtime_error("\"" + name + "." + pair.first + "\" must be a" + typeToStr(valuesTypes[pair.first]) + " (it is currently a" + typeToStr(object[pair.first].type()) + ").");
		}
	}
}

void parseObject(const Json::Value& object, const std::string name, Json::ValueType type)
{
	if(object.type() != Json::objectValue)
		throw std::runtime_error("\"" + name + "\" must be an object.");
	else
	{
		for(std::string& elementName : object.getMemberNames())
		{
			if(not object[elementName].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." + elementName + "\" must be a" + typeToStr(type) + ".");
		}
	}
}

void parseArray(const Json::Value& array, const std::string name, std::vector<Json::Value> values)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
	{
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
		{
			if(std::find(values.begin(), values.end(), array[i]) == values.end())//If the value in the array is not in the vector
				throw std::runtime_error("\"" + name + "." + std::to_string(i) + "\" identifier (" + array[i].asString() + ") is not recognized.");
		}
	}
}

void parseValue(const Json::Value& value, const std::string name, std::vector<Json::Value> values)
{
	if(std::find(values.begin(), values.end(), value) == values.end())//If the value is not in the vector
		throw std::runtime_error("\"" + name + "\" value (" + value.asString() + ") is not recognized.");
}

void parseValue(const Json::Value& value, const std::string name, Json::ValueType type)
{
	if(not value.isConvertibleTo(type))
		throw std::runtime_error("\"" + name + "\" must be a" + typeToStr(type) + ".");
}

void parseArray(const Json::Value& array, const std::string name, Json::ValueType type)
{
	if(array.type() != Json::arrayValue)
		throw std::runtime_error("\"" + name + "\" must be an array.");
	else
	{
		for(Json::ArrayIndex i{0}; i < array.size(); ++i)
		{
			if(not array[i].isConvertibleTo(type))
				throw std::runtime_error("\"" + name + "." +std::to_string(i) + "\" must be a" + typeToStr(type) + ".");
		}
	}
}

void parse(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName)
{
	//Assert that the type is one of those
	parseValue(model["type"], modelName + ".type", {"string", "object", "array", "real", "int", "uint", "bool", "null"});
	//Assert that the value has the right type
	parseValue(value, valueName, strToType(model["type"].asString()));
	if(model["type"].asString() == "object")
	{
		requireValues(model, modelName, {{"free children names", Json::booleanValue}});
		if(model["free children names"].asBool())
		{
			parseObject(model, modelName, {{"free children names", Json::booleanValue},
											{"name", Json::stringValue},
											{"required", Json::booleanValue},
											{"type", Json::stringValue},
											{"children", Json::objectValue}});
			//If names are free, children must be an object
			requireValues(model, modelName, {{"children", Json::objectValue}});
			requireValues(model["children"], modelName + ".children", {{"type", Json::stringValue}});
			//For every child of value
			for(std::string valueChildName : value.getMemberNames())
				//Parse the child according to the model child
				parse(value[valueChildName], model["children"], valueName + "." + valueChildName, modelName + ".children");
		}
		else
		{
			parseObject(model, modelName, {{"free children names", Json::booleanValue},
											{"name", Json::stringValue},
											{"required", Json::booleanValue},
											{"type", Json::stringValue},
											{"children", Json::arrayValue}});
			//If names are not free, children must be an array
			requireValues(model, modelName, {{"children", Json::arrayValue}});
			parseArray(model["children"], modelName, Json::objectValue);
			std::map<std::string, Json::ValueType> requiredChildren;
			std::map<std::string, Json::ValueType> possibleChildren;
			for(unsigned int i{0}; i < model["children"].size(); ++i)
			{
				Json::Value modelChild{model["children"][i]};
				//A not-free-name child must have these three members defined
				//Require name before, and if the name is defined we can require the others member with a fancier exception than ".children.i"
				requireValues(modelChild, modelName + ".children." + std::to_string(i), {{"name", Json::stringValue}});
				const std::string childName{modelChild["name"].asString()};
				requireValues(modelChild, modelName + "." + childName, {{"required", Json::booleanValue},
																		{"type", Json::stringValue}});
				//If the current child is required
				if(modelChild["required"].asBool())
					//Add it to the required children map
					requiredChildren.emplace(childName, strToType(modelChild["type"].asString()));
				//In all case, add the child to the possible children map
				possibleChildren.emplace(childName, strToType(modelChild["type"].asString()));
				if(value.isMember(childName))
					parse(value[childName], modelChild, valueName + "." + childName, modelName + "." + childName);
			}
			requireValues(value, valueName, requiredChildren);
			parseObject(value, valueName, possibleChildren);
		}
	}
	else if(model["type"].asString() == "array")
	{
		parseObject(model, modelName, {{"name", Json::stringValue},
										{"required", Json::booleanValue},
										{"type", Json::stringValue},
										{"children", Json::objectValue}});
		requireValues(model, modelName, {{"children", Json::objectValue}});
		//For each element in value
		for(unsigned int i{0}; i < value.size(); ++i)
			//Parse it according to the model child
			parse(value[i], model["children"], valueName + "." + std::to_string(i), modelName);
	}
	//Base type
	else
	{
		//Assert that the value has only thoses members defined
		parseObject(model, modelName, {{"default value", strToType(model["type"].asString())},
										{"name", Json::stringValue},
										{"required", Json::booleanValue},
										{"type", Json::stringValue},
										{"possible values", Json::arrayValue}});
		//If default and required = true defined at the same time
		if(model.isMember("default value") and model.get("required", false).asBool())
			throw std::runtime_error("\"" + modelName + ".default\" is defined but \"" + modelName + ".required\" is set to true");
		//If value is a null value, so if the value do not exists, and if there is a default value
		if(value == Json::Value(Json::nullValue) and model.isMember("default value"))
			value = model["default value"];

		//Assert that the value is one of the possible values
		if(model.isMember("possible values"))
		{
			std::vector<Json::Value> possibleValues;
			for(unsigned int i{0}; i < model["possible values"].size(); ++i)
				possibleValues.push_back(model["possible values"][i]);
			parseValue(value, valueName, possibleValues);
		}
	}
}
