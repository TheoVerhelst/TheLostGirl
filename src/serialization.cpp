#include <dist/json/json.h>
#include <entityx/Entity.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/FixtureRoles.h>

#include <TheLostGirl/serialization.h>

Json::Value serialize(entityx::ComponentHandle<BodyComponent> component, float scale)
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
									
		ret[partName]["position"]["x"] = body->GetPosition().x/scale;
		ret[partName]["position"]["y"] = body->GetPosition().y/scale;
		ret[partName]["angle"] = body->GetAngle();
		ret[partName]["linear velocity"]["x"] = body->GetLinearVelocity().x/scale;
		ret[partName]["linear velocity"]["y"] = body->GetLinearVelocity().y/scale;
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
					b2CircleShape* shape = static_cast<b2CircleShape*>(fix->GetShape());
					fixtureObj["position"]["x"] = shape->m_p.x/scale;
					fixtureObj["position"]["y"] = shape->m_p.y/scale;
					fixtureObj["radius"] = shape->m_radius/scale;
					break;
				}
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = static_cast<b2EdgeShape*>(fix->GetShape());
					//Copy all vertices
					if(shape->m_hasVertex0)
					{
						fixtureObj["0"]["x"] = shape->m_vertex0.x/scale;
						fixtureObj["0"]["y"] = shape->m_vertex0.y/scale;
					}
					fixtureObj["1"]["x"] = shape->m_vertex1.x/scale;
					fixtureObj["1"]["y"] = shape->m_vertex1.y/scale;
					fixtureObj["2"]["x"] = shape->m_vertex2.x/scale;
					fixtureObj["2"]["y"] = shape->m_vertex2.y/scale;
					if(shape->m_hasVertex3)
					{
						fixtureObj["3"]["x"] = shape->m_vertex3.x/scale;
						fixtureObj["3"]["y"] = shape->m_vertex3.y/scale;
					}
					break;
				}
				case b2Shape::e_polygon:
				{
					b2PolygonShape* shape = static_cast<b2PolygonShape*>(fix->GetShape());
					//Copy all vertices
					for(int32 j{0}; j < shape->GetVertexCount(); ++j)
					{
						fixtureObj["vertices"][j]["x"] = shape->m_vertices[j].x/scale;
						fixtureObj["vertices"][j]["y"] = shape->m_vertices[j].y/scale;
					}
					break;
				}
				default:
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = static_cast<b2ChainShape*>(fix->GetShape());
					//Copy all vertices
					if(shape->m_hasPrevVertex)
					{
						fixtureObj["previous vertex"]["x"] = shape->m_prevVertex.x/scale;
						fixtureObj["previous vertex"]["y"] = shape->m_prevVertex.y/scale;
					}
					for(int32 j{0}; j < shape->m_count; ++j)
					{
						fixtureObj["vertices"][j]["x"] = shape->m_vertices[j].x/scale;
						fixtureObj["vertices"][j]["y"] = shape->m_vertices[j].y/scale;
					}
					if(shape->m_hasNextVertex)
					{
						fixtureObj["next vertex"]["x"] = shape->m_nextVertex.x/scale;
						fixtureObj["next vertex"]["y"] = shape->m_nextVertex.y/scale;
					}
					break;
				}
			}
			fixtureObj["density"] = fix->GetDensity();
			fixtureObj["friction"] = fix->GetFriction();
			fixtureObj["restitution"] = fix->GetRestitution();
			fixtureObj["is sensor"] = fix->IsSensor();
			//If the fixture is a foot sensor
			if(fix->GetUserData() and (unsigned int)(fix->GetUserData()) & FixtureRole::Foot)
				fixtureObj["is foot sensor"] = true;
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
					
				default:
				case b2Shape::e_chain:
					ret[partName]["chain fixtures"].append(fixtureObj);
					break;
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
		const std::string partName{spritePair.first};
		const sf::Texture* tex = spritePair.second.getTexture();
		//Compare the pointer to the texture
		if(tex == &textureManager.get("archer"))
			ret[partName]["identifier"] = "archer";
		else if(tex == &textureManager.get("arms"))
			ret[partName]["identifier"] = "arms";
		else if(tex == &textureManager.get("bow"))
			ret[partName]["identifier"] = "bow";
	}
	for(auto& positionPair : component->worldPositions)
	{
		const std::string partName{positionPair.first};
		ret[partName]["position"]["x"] = positionPair.second.x/scale;
		ret[partName]["position"]["y"] = positionPair.second.y/scale;
		ret[partName]["position"]["z"] = positionPair.second.z;
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
		
		//polygon fixtures
		if(body.isMember("polygon fixtures"))
		{
			const Json::Value fixtures = body["polygon fixtures"];
			for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
			{
				const Json::Value fixture = fixtures[i];
				b2FixtureDef entityFixtureDef;
				b2PolygonShape polygonShape;
				
				//vertices
				const Json::Value vertices = fixtures[i]["vertices"];
				std::vector<b2Vec2> verticesVec(vertices.size());
				for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
				{
					const Json::Value vertice = vertices[j];
				
					//x
					verticesVec[j].x = vertice["x"].asFloat()*scale;
					
					//y
					verticesVec[j].y = vertice["y"].asFloat()*scale;
				}
				polygonShape.Set(verticesVec.data(), verticesVec.size());
				entityFixtureDef.shape = &polygonShape;
				
				//density
				entityFixtureDef.density = fixture["density"].asFloat();
				
				//friction
				entityFixtureDef.friction = fixture["friction"].asFloat();
				
				//restitution
				entityFixtureDef.restitution = fixture["restitution"].asFloat();
				
				//is sensor
				entityFixtureDef.isSensor = fixture["is sensor"].asBool();
				
				//is foot sensor
				entityFixtureDef.userData = (void*)FixtureRole::Foot;
					
				entityBodyComponent->CreateFixture(&entityFixtureDef);
			}
		}
		
		//edge fixtures
		if(body.isMember("edge fixtures"))
		{
			const Json::Value fixtures = body["edge fixtures"];
			for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
			{
				const Json::Value fixture = fixtures[i];
								
				b2FixtureDef entityFixtureDef;
				b2EdgeShape edgeShape;
				b2Vec2 one{0, 0}, two{0, 0};
				
				//x 1
				one.x = fixtures[i]["1"]["x"].asFloat();
				
				//y 1
				one.y = fixtures[i]["1"]["y"].asFloat();
				
				//x 2
				two.x = fixtures[i]["2"]["x"].asFloat();
				
				//y 2
				two.y = fixtures[i]["2"]["y"].asFloat();
				
				edgeShape.Set(scale*one, scale*two);
				
				//0
				if(fixture.isMember("0"))
				{
					edgeShape.m_hasVertex0 = true;
					
					//x
					edgeShape.m_vertex0.x = fixtures[i]["0"]["x"].asFloat()*scale;
					
					//y
					edgeShape.m_vertex0.y = fixtures[i]["0"]["y"].asFloat()*scale;
				}
				
				//3
				if(fixture.isMember("3"))
				{
					edgeShape.m_hasVertex3 = true;
					
					//x
					edgeShape.m_vertex3.x = fixtures[i]["3"]["x"].asFloat()*scale;
					
					//y
					edgeShape.m_vertex3.y = fixtures[i]["3"]["y"].asFloat()*scale;
				}
				entityFixtureDef.shape = &edgeShape;
				
				//density
				entityFixtureDef.density = fixture["density"].asFloat();
				
				//friction
				entityFixtureDef.friction = fixture["friction"].asFloat();
				
				//restitution
				entityFixtureDef.restitution = fixture["restitution"].asFloat();
				
				//is sensor
				entityFixtureDef.isSensor = fixture["is sensor"].asBool();
				
				//is foot sensor
				entityFixtureDef.userData = (void*)FixtureRole::Foot;
					
				entityBodyComponent->CreateFixture(&entityFixtureDef);
			}
		}
		
		//chain fixtures
		if(body.isMember("chain fixtures"))
		{
			const Json::Value fixtures = body["chain fixtures"];
			for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
			{
				const Json::Value fixture = fixtures[i];
				b2FixtureDef entityFixtureDef;
				b2ChainShape chainShape;
				std::vector<b2Vec2> verticesArray;
				const Json::Value vertices = fixtures[i]["vertices"];
				//For each vertex of the chain shape
				for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
				{
					const Json::Value vertice = vertices[j];
					verticesArray.push_back(b2Vec2(0, 0));
					//x
					verticesArray[j].x = vertice["x"].asFloat()*scale;
					
					//y
					verticesArray[j].y = vertice["y"].asFloat()*scale;
				}
				chainShape.CreateChain(verticesArray.data(), verticesArray.size());
				
				//previous vertex
				if(fixtures[i].isMember("previous vertex"))
				{
					chainShape.m_hasPrevVertex = true;
					
					//x
					chainShape.m_prevVertex.x = fixtures[i]["previous vertex"]["x"].asFloat()*scale;
					
					//y
					chainShape.m_prevVertex.y = fixtures[i]["previous vertex"]["y"].asFloat()*scale;
				}
				
				//next vertex
				if(fixtures[i].isMember("next vertex"))
				{
					chainShape.m_hasNextVertex = true;
					
					//x
					chainShape.m_nextVertex.x = fixtures[i]["next vertex"]["x"].asFloat()*scale;
					
					//y
					chainShape.m_nextVertex.y = fixtures[i]["next vertex"]["y"].asFloat()*scale;
				}
				entityFixtureDef.shape = &chainShape;
				
				//density
				entityFixtureDef.density = fixture["density"].asFloat();
				
				//friction
				entityFixtureDef.friction = fixture["friction"].asFloat();
				
				//restitution
				entityFixtureDef.restitution = fixture["restitution"].asFloat();
				
				//is sensor
				entityFixtureDef.isSensor = fixture["is sensor"].asBool();
				
				//is foot sensor
				entityFixtureDef.userData = (void*)FixtureRole::Foot;
					
				entityBodyComponent->CreateFixture(&entityFixtureDef);
			}
		}
		
		//circle fixtures
		if(body.isMember("circle fixtures"))
		{
			const Json::Value fixtures = body["circle fixtures"];
			for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
			{
				const Json::Value fixture = fixtures[i];
								
				b2FixtureDef entityFixtureDef;
				b2CircleShape circleShape;
				
				//x
				circleShape.m_p.x = fixtures[i]["position"]["x"].asFloat()*scale;
				
				//y
				circleShape.m_p.y = fixtures[i]["position"]["y"].asFloat()*scale;
				
				//radius
				circleShape.m_radius = fixtures[i]["radius"].asFloat()*scale;
				entityFixtureDef.shape = &circleShape;
				
				//density
				entityFixtureDef.density = fixture["density"].asFloat();
				
				//friction
				entityFixtureDef.friction = fixture["friction"].asFloat();
				
				//restitution
				entityFixtureDef.restitution = fixture["restitution"].asFloat();
				
				//is sensor
				entityFixtureDef.isSensor = fixture["is sensor"].asBool();
				
				//is foot sensor
				entityFixtureDef.userData = (void*)FixtureRole::Foot;
					
				entityBodyComponent->CreateFixture(&entityFixtureDef);
			}
		}
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, TextureManager& textureManager, float scale)
{
	component->sprites.clear();
	component->worldPositions.clear();
	for(std::string& partName : value.getMemberNames())
	{
		Json::Value part = value[partName];
		component->sprites.emplace(partName, sf::Sprite(textureManager.get(part["identifier"].asString())));
		sf::Vector3f position;
		position.x = part["position"]["x"].asFloat()*scale;
		position.y = part["position"]["y"].asFloat()*scale;
		position.z = part["position"]["z"].asFloat();
		component->worldPositions.emplace(partName, position);
	}
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent, State::Context context)
{
	component->animationsManagers.clear();
	for(std::string& partName : value.getMemberNames())
	{
		//If the associated sprite exists
		if(spriteComponent->sprites.find(partName) != spriteComponent->sprites.end())
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
	unsigned int category;
	for(Json::ArrayIndex i{0}; i < value.size(); ++i)
	{
		if(value[i] == "player")
			category |= Category::Player;
		else if(value[i] == "scene")
			category |= Category::Scene;
	}
	component->category = category;
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
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<BendComponent> component)
{
	if(value.isMember("power"))
		component->power = value["power"].asFloat();
	if(value.isMember("angle"))
		component->angle = value["angle"].asFloat();
	component->maxPower = value["maximum power"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component)
{
	if(value.isMember("current health"))
		component->health = value["current health"].asFloat();
	component->maxHealth = value["maximum health"].asFloat();
}

void deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component)
{
	if(value.isMember("current stamina"))
		component->stamina = value["current stamina"].asFloat();
	component->maxStamina = value["maximum stamina"].asFloat();
}
