#include <dist/json/json.h>
#include <entityx/Entity.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/FixtureRoles.h>

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

void deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, b2World& world, float scale)
{
	component->bodies.clear();
	for(std::string& partName : value.getMemberNames())
	{
		Json::Value body = value[partName];
		b2BodyDef entityBodyComponentDef;
		
		//type
		if(body.isMember("type"))
		{
			const Json::Value type = body["type"];
			if(type == "static")
				entityBodyComponentDef.type = b2_staticBody;
			else if(type == "kinematic")
				entityBodyComponentDef.type = b2_kinematicBody;
			else if(type == "dynamic")
				entityBodyComponentDef.type = b2_dynamicBody;
		}
		
		//position
		if(body.isMember("position"))
		{
			const Json::Value position = body["position"];
			if(position.isMember("x"))
				entityBodyComponentDef.position.x = position["x"].asFloat()*scale;
			if(position.isMember("y"))
				entityBodyComponentDef.position.y = position["y"].asFloat()*scale;
		}
		
		//angle
		if(body.isMember("angle"))
			entityBodyComponentDef.angle = body["angle"].asFloat();
		
		//linear velocity
		if(body.isMember("linear velocity"))
		{
			const Json::Value linearVelocity = body["linear velocity"];
			if(linearVelocity.isMember("x"))
				entityBodyComponentDef.linearVelocity.x = linearVelocity["x"].asFloat()*scale;
			if(linearVelocity.isMember("y"))
				entityBodyComponentDef.linearVelocity.y = linearVelocity["y"].asFloat()*scale;
		}
		
		//angular velocity
		if(body.isMember("angular velocity"))
			entityBodyComponentDef.angularVelocity = body["angular velocity"].asFloat();
		
		//linear damping
		if(body.isMember("linear damping"))
			entityBodyComponentDef.linearDamping = body["linear damping"].asFloat();
		
		//angular damping
		if(body.isMember("angular damping"))
			entityBodyComponentDef.angularDamping = body["angular damping"].asFloat();
		
		//allow sleep
		if(body.isMember("allow sleep"))
			entityBodyComponentDef.allowSleep = body["allow sleep"].asBool();
		
		//awake
		if(body.isMember("awake"))
			entityBodyComponentDef.awake = body["awake"].asBool();
		
		//fixed rotation
		if(body.isMember("fixed rotation"))
			entityBodyComponentDef.fixedRotation = body["fixed rotation"].asBool();
		
		//bullet
		if(body.isMember("bullet"))
			entityBodyComponentDef.bullet = body["bullet"].asBool();
		
		//active
		if(body.isMember("active"))
			entityBodyComponentDef.active = body["active"].asBool();
		
		//gravity scale
		if(body.isMember("gravity scale"))
			entityBodyComponentDef.gravityScale = body["gravity scale"].asFloat();
		
		b2Body* entityBodyComponent = world.CreateBody(&entityBodyComponentDef);
		component->bodies.emplace(partName, entityBodyComponent);
		
		//fixtures
		if(body.isMember("fixtures"))
		{
			const Json::Value fixtures = body["fixtures"];
			for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
			{
				const Json::Value fixture = fixtures[i];
								
				b2FixtureDef entityFixtureDef;
				//Initialize shapes here in order to keep pointers alive when assigning entityFixtureDef.shape
				b2EdgeShape edgeShape;
				b2PolygonShape polygonShape;
				b2ChainShape chainShape;
				std::vector<b2Vec2> verticesArray;
				
				//type
				if(fixture.isMember("type"))
				{
					const Json::Value type = fixtures[i]["type"];

					if(type == "polygon")
					{
						//box
						if(fixture.isMember("box"))
						{
							const Json::Value box = fixtures[i]["box"];
							float w{100}, h{100};
							float angle{0};
							
							//width
							if(box.isMember("w"))
								w = box["w"].asFloat();
							
							//height
							if(box.isMember("h"))
								h = box["h"].asFloat();
							
							float cx{w/2}, cy{h/2};
							
							//center x
							if(box.isMember("center x"))
								cx = box["center x"].asFloat();
							
							//center y
							if(box.isMember("center y"))
								cy = box["center y"].asFloat();
							
							//angle
							if(box.isMember("angle"))
								angle = box["angle"].asFloat();
								
							polygonShape.SetAsBox((w*scale)/2, (h*scale)/2, {cx*scale, cy*scale}, angle);
						}
						
						//vertices
						else if(fixture.isMember("vertices"))
						{
							const Json::Value vertices = fixtures[i]["vertices"];
							for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
							{
								const Json::Value vertice = vertices[j];
							
								//x
								if(vertice.isMember("x"))
									polygonShape.m_vertices[j].x = vertice["x"].asFloat()*scale;
								
								//y
								if(vertice.isMember("y"))
									polygonShape.m_vertices[j].y = vertice["y"].asFloat()*scale;
							}
						}
						entityFixtureDef.shape = &polygonShape;
					}
					else if(type == "edge")
					{
						float x1{0}, y1{0}, x2{0}, y2{0};
						//x 1
						if(fixture["1"].isMember("x"))
							x1 = fixtures[i]["1"]["x"].asFloat();
						//y 1
						if(fixture["1"].isMember("y"))
							y1 = fixtures[i]["1"]["y"].asFloat();
						
						//x 2
						if(fixture["2"].isMember("x"))
							x2 = fixtures[i]["2"]["x"].asFloat();
						//y 2
						if(fixture["2"].isMember("y"))
							y2 = fixtures[i]["2"]["y"].asFloat();
						
						edgeShape.Set({(x1*scale), (y1*scale)}, {(x2*scale), (y2*scale)});
						entityFixtureDef.shape = &edgeShape;
					}
					else if(type == "chain")
					{
						//vertices
						const Json::Value vertices = fixtures[i]["vertices"];
						//Vertices of the chain shape
						for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
						{
							const Json::Value vertice = vertices[j];
							verticesArray.push_back(b2Vec2(0, 0));
							//x
							if(vertice.isMember("x"))
								verticesArray[j].x = vertice["x"].asFloat()*scale;
							
							//y
							if(vertice.isMember("y"))
								verticesArray[j].y = vertice["y"].asFloat()*scale;
						}
						chainShape.CreateChain(verticesArray.data(), verticesArray.size());
						entityFixtureDef.shape = &chainShape;
					}
					// TODO: implementer l'autre type de shapes.
					else if(type == "circle")
					{
					}
				}
				
				//density
				if(fixture.isMember("density"))
					entityFixtureDef.density = fixture["density"].asFloat();
				
				//friction
				if(fixture.isMember("friction"))
					entityFixtureDef.friction = fixture["friction"].asFloat();
				
				//restitution
				if(fixture.isMember("restitution"))
					entityFixtureDef.restitution = fixture["restitution"].asFloat();
				
				//is sensor
				if(fixture.isMember("is sensor"))
					entityFixtureDef.isSensor = fixture["is sensor"].asBool();
				
				//is foot sensor
				if(fixture.isMember("is foot sensor") and fixture["is foot sensor"].asBool())
					entityFixtureDef.userData = (void*)FixtureRole::Foot;
					
				entityBodyComponent->CreateFixture(&entityFixtureDef);
			}
		}
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager)
{
	component->sprites.clear();
	component->worldPositions.clear();
	for(std::string& partName : value.getMemberNames())
	{
		Json::Value part = value[partName];
		component->sprites.emplace(partName, sf::Sprite(textureManager.get(part["identifier"].asString())));
		sf::Vector2f position;
		position.x = part["position"]["x"].asFloat();
		position.y = part["position"]["y"].asFloat();
		component->worldPositions.emplace(partName, position);
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent)
{
	component->animationsManagers.clear();
	for(std::string& partName : value.getMemberNames())
	{
		//If the associated sprite exists
		if(spriteComponent->sprites.find(partName) != spriteComponent->sprites.end())
		{
			AnimationsManager<SpriteSheetAnimation> animationsManager;
			animationsManager.deserialize(value[partName], spriteComponent->sprites[partName]);
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
	else
		component->direction = Direction::None;
	component->moveToLeft = value["move to left"].asBool();
	component->moveToRight = value["move to right"].asBool();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component)
{
	unsigned int category;
	for(Json::ArrayIndex i{0}; i < value.size(); ++i)
	{
		if(value[i] == "player")
			category &= Category::Player;
		else if(value[i] == "scene")
			category &= Category::Scene;
	}
	component->category = category
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<ActorIDComponent> component)
{
	component->ID = value.asUInt();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<SkyComponent> component)
{
	component->day = value.asBool();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component)
{
	component->inAir = value["in air"].asBool();
	component->contactCount = value["contact count"].asUInt();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component)
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
	component->walkSpeed = value["speed"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component)
{
	component->jumpStrength = value["strength"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<BendComponent> component)
{
	component->power = value["power"].asFloat();
	component->angle = value["angle"].asFloat();
	component->maxPower = value["maximum power"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component)
{
	component->health = value["current health"].asFloat();
	component->maxHealth = value["maximum health"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component)
{
	component->stamina = value["current stamina"].asFloat();
	component->maxStamina = value["maximum stamina"].asFloat();
}
