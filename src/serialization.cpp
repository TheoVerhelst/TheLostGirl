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

Json::Value Serializer::serialize(entityx::ComponentHandle<BodyComponent> component)
{
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	Json::Value ret;
	switch(component->body->GetType())
	{
		case b2_kinematicBody:
			ret["type"] = "kinematic";
			break;
		case b2_dynamicBody:
			ret["type"] = "dynamic";
			break;
		case b2_staticBody:
		default:
			ret["type"] = "static";
			break;
	}

	ret["linear velocity"]["x"] = component->body->GetLinearVelocity().x*pixelByMeter;
	ret["linear velocity"]["y"] = component->body->GetLinearVelocity().y*pixelByMeter;
	ret["angular velocity"] = component->body->GetAngularVelocity();
	ret["linear damping"] = component->body->GetLinearDamping();
	ret["angular damping"] = component->body->GetAngularDamping();
	ret["allow sleep"] = component->body->IsSleepingAllowed();
	ret["awake"] = component->body->IsAwake();
	ret["fixed rotation"] = component->body->IsFixedRotation();
	ret["bullet"] = component->body->IsBullet();
	ret["active"] = component->body->IsActive();
	ret["gravity scale"] = component->body->GetGravityScale();

	//For each fixture of the component->body
	for(b2Fixture* fix{component->body->GetFixtureList()}; fix; fix = fix->GetNext())
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
				ret["circle fixtures"].append(fixtureObj);
				break;

			case b2Shape::e_edge:
				ret["edge fixtures"].append(fixtureObj);
				break;

			case b2Shape::e_polygon:
				ret["polygon fixtures"].append(fixtureObj);
				break;

			case b2Shape::e_chain:
				ret["chain fixtures"].append(fixtureObj);
				break;

			case b2Shape::e_typeCount:
			default:
				throw std::runtime_error("Invalid shape type.");
		}
	}
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<SpriteComponent> component)
{
	Json::Value ret;
	const sf::Texture* tex{component->sprite.getTexture()};//Get the associated texture
	ret["identifier"] = textureManager.getIdentifier(*tex);//Get the identifier of the texture
	if(component->sprite.getOrigin() != sf::Vector2f(0, 0))
	{
		ret["origin"]["x"] = component->sprite.getOrigin().x/m_context.parameters.scale;
		ret["origin"]["y"] = component->sprite.getOrigin().y/m_context.parameters.scale;
	}
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<TransformComponent> component)
{
	Json::Value ret;
	ret["x"] = component->transform.x;
	ret["y"] = component->transform.y;
	ret["z"] = component->transform.z;
	ret["angle"] = component->transform.angle;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<InventoryComponent> component)
{
	Json::Value ret;
	ret["maximum weight"] = component->maxWeight;
	ret["items"] = Json::Value(Json::arrayValue);
	//For each item of the inventory
	for(entityx::Entity item : component->items)
		//If the item is in the entity list
		if(isMember(m_entitiesMap, item) and item.valid())
			//Add it to the value
			ret["items"].append(getKey(m_entitiesMap, item));
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component)
{
	return component->animationsManager.serialize();
}

Json::Value Serializer::serialize(entityx::ComponentHandle<DirectionComponent> component)
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
			break;
	}
	if(component->moveToLeft)
		ret["move to left"] = true;
	if(component->moveToRight)
		ret["move to right"] = true;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<CategoryComponent> component)
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

Json::Value Serializer::serialize(entityx::ComponentHandle<FallComponent> component)
{
	Json::Value ret;
	ret["falling resistance"] = component->fallingResistance;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<WalkComponent> component)
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

Json::Value Serializer::serialize(entityx::ComponentHandle<JumpComponent> component)
{
	Json::Value ret;
	ret["strength"] = component->jumpStrength;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<ArcherComponent> component)
{
	Json::Value ret;
	ret["damages"] = component->damages;
	ret["initial speed"] = component->initialSpeed;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<HealthComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<StaminaComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<ArrowComponent> component)
{
	Json::Value ret;
	ret["friction"] = component->friction;
	ret["penetrance"] = component->penetrance;
	ret["damage"] = component->damage;
	ret["local friction point"]["x"] = component->localFrictionPoint.x;
	ret["local friction point"]["y"] = component->localFrictionPoint.y;
	ret["local stick point"]["x"] = component->localStickPoint.x;
	ret["local stick point"]["y"] = component->localStickPoint.y;
	if(component->shooter and isMember(m_entitiesMap, component->shooter))
		ret["shooter"] = getKey(m_entitiesMap, component->shooter);
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

Json::Value Serializer::serialize(entityx::ComponentHandle<HardnessComponent> component)
{
	return component->hardness;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<ScriptsComponent> component)
{
	Json::Value ret;
	ret["scripts"] = Json::Value(Json::arrayValue);
	for(auto& scriptName : component->scriptsNames)
        ret["scripts"].append(scriptName);
    return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<DetectionRangeComponent> component)
{
	return component->detectionRange;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<DeathComponent> component)
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

Json::Value Serializer::serialize(entityx::ComponentHandle<NameComponent> component)
{
	return component->name;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<HandToHandComponent> component)
{
	Json::Value ret;
	ret["damages"] = component->damages;
	ret["delay"] = component->delay.asSeconds();
	ret["last shoot"] = component->lastShoot.asSeconds();
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<ActorComponent> component)
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

Json::Value Serializer::serialize(entityx::ComponentHandle<HoldItemComponent> component)
{
	Json::Value ret;
	if(isMember(m_entitiesMap, component->item))
		ret["item"] = getKey(m_entitiesMap, component->item);
	const b2Vec2 localAnchor{component->armsJoint->GetLocalAnchorA()*m_context.parameters.pixelByMeter};
	ret["local anchor"]["x"] = localAnchor.x;
	ret["local anchor"]["y"] = localAnchor.y;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<ArticuledArmsComponent> component)
{
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	Json::Value ret;
	if(isMember(m_entitiesMap, component->arms))
		ret["arms"] = getKey(m_entitiesMap, component->arms);
	ret["target angle"] = component->targetAngle * 180.f / b2_pi;
	ret["upper angle"] = component->armsJoint->GetUpperLimit() * 180.f / b2_pi;
	ret["lower angle"] = component->armsJoint->GetLowerLimit() * 180.f / b2_pi;
	ret["current angle"] = component->armsJoint->GetJointAngle() * 180.f / b2_pi;
	const b2Vec2 localAnchor{component->armsJoint->GetLocalAnchorA()*pixelByMeter};
	ret["local anchor"]["x"] = localAnchor.x;
	ret["local anchor"]["y"] = localAnchor.y;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<BowComponent> component)
{
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	Json::Value ret;
	if(isMember(m_entitiesMap, component->notchedArrow) and component->notchedArrow.valid())
		ret["notched arrow"] = getKey(m_entitiesMap, component->notchedArrow);
	ret["target translation"] = component->targetTranslation;
	ret["upper translation"] = component->notchedArrowJoint->GetUpperLimit()*pixelByMeter;
	ret["lower translation"] = component->notchedArrowJoint->GetLowerLimit()*pixelByMeter;
	ret["current translation"] = component->notchedArrowJoint->GetJointTranslation()*pixelByMeter;
	const b2Vec2 localAnchor{component->notchedArrowJoint->GetLocalAnchorA()*pixelByMeter};
	ret["local anchor"]["x"] = localAnchor.x;
	ret["local anchor"]["y"] = localAnchor.y;
	return ret;
}

Json::Value Serializer::serialize(entityx::ComponentHandle<QuiverComponent> component)
{
	Json::Value ret;
	ret["capacity"] = component->capacity;
	ret["arrows"] = Json::Value(Json::arrayValue);
	//For each arrow of the quiver
	for(entityx::Entity arrow : component->arrows)
		//If the arrow is in the entity list
		if(isMember(m_entitiesMap, arrow) and arrow.valid())
			//Add it to the value
			ret["arrows"].append(getKey(m_entitiesMap, arrow));
	return ret;
}

//End serialize

  //////////               //////////
 ////////// Deserializing //////////
//////////               //////////

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::ComponentHandle<TransformComponent> transformComponent)
{
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	b2BodyDef bodyDef;
	//type
	const Json::Value type{value["type"]};
	if(type == "static")
		bodyDef.type = b2_staticBody;
	else if(type == "kinematic")
		bodyDef.type = b2_kinematicBody;
	else if(type == "dynamic")
		bodyDef.type = b2_dynamicBody;

	//position
	bodyDef.position.x = transformComponent->transform.x/pixelByMeter;
	bodyDef.position.y = transformComponent->transform.y/pixelByMeter;

	//angle
	bodyDef.angle = transformComponent->transform.angle*b2_pi/180;
	const Json::Value linearVelocity{value["linear velocity"]};
	bodyDef.linearVelocity.x = linearVelocity["x"].asFloat()/pixelByMeter;
	bodyDef.linearVelocity.y = linearVelocity["y"].asFloat()/pixelByMeter;
	bodyDef.angularVelocity = value["angular velocity"].asFloat();
	bodyDef.linearDamping = value["linear damping"].asFloat();
	bodyDef.angularDamping = value["angular damping"].asFloat();
	bodyDef.allowSleep = value["allow sleep"].asBool();
	bodyDef.awake = value["awake"].asBool();
	bodyDef.fixedRotation = value["fixed rotation"].asBool();
	bodyDef.bullet = value["bullet"].asBool();
	bodyDef.active = value["active"].asBool();
	bodyDef.gravityScale = value["gravity scale"].asFloat();

	component->body = world.CreateBody(&bodyDef);

	//polygon fixtures
	if(value.isMember("polygon fixtures"))
	{
		const Json::Value fixtures{value["polygon fixtures"]};
		for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
		{
			const Json::Value fixture{fixtures[i]};
			b2FixtureDef fixtureDef;
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
			fixtureDef.shape = &polygonShape;

			fixtureDef.density = fixture["density"].asFloat();
			fixtureDef.friction = fixture["friction"].asFloat();
			fixtureDef.restitution = fixture["restitution"].asFloat();
			fixtureDef.isSensor = fixture["is sensor"].asBool();
			const Json::Value roles{fixture["roles"]};
			for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
			{
				if(roles[j].asString() == "foot sensor")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
				else if(roles[j].asString() == "main")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
			}
			component->body->CreateFixture(&fixtureDef);
		}
	}

	//edge fixtures
	if(value.isMember("edge fixtures"))
	{
		const Json::Value fixtures{value["edge fixtures"]};
		for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
		{
			const Json::Value fixture{fixtures[i]};

			b2FixtureDef fixtureDef;
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
			fixtureDef.shape = &edgeShape;
			fixtureDef.density = fixture["density"].asFloat();
			fixtureDef.friction = fixture["friction"].asFloat();
			fixtureDef.restitution = fixture["restitution"].asFloat();
			fixtureDef.isSensor = fixture["is sensor"].asBool();

			//roles
			const Json::Value roles{value["roles"]};
			for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
			{
				if(roles[j].asString() == "foot sensor")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
				else if(roles[j].asString() == "main")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
			}

			component->body->CreateFixture(&fixtureDef);
		}
	}

	//chain fixtures
	if(value.isMember("chain fixtures"))
	{
		const Json::Value fixtures{value["chain fixtures"]};
		for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
		{
			const Json::Value fixture{fixtures[i]};
			b2FixtureDef fixtureDef;
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
			fixtureDef.shape = &chainShape;
			fixtureDef.density = fixture["density"].asFloat();
			fixtureDef.friction = fixture["friction"].asFloat();
			fixtureDef.restitution = fixture["restitution"].asFloat();
			fixtureDef.isSensor = fixture["is sensor"].asBool();
			const Json::Value roles{value["roles"]};
			for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
			{
				if(roles[j].asString() == "foot sensor")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
				else if(roles[j].asString() == "main")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
			}

			component->body->CreateFixture(&fixtureDef);
		}
	}

	//circle fixtures
	if(value.isMember("circle fixtures"))
	{
		const Json::Value fixtures{value["circle fixtures"]};
		for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
		{
			const Json::Value fixture{fixtures[i]};

			b2FixtureDef fixtureDef;

			b2CircleShape circleShape;
			circleShape.m_p.x = fixtures[i]["position"]["x"].asFloat()/pixelByMeter;
			circleShape.m_p.y = fixtures[i]["position"]["y"].asFloat()/pixelByMeter;

			fixtureDef.shape = &circleShape;
			fixtureDef.density = fixture["density"].asFloat();
			fixtureDef.friction = fixture["friction"].asFloat();
			fixtureDef.restitution = fixture["restitution"].asFloat();
			fixtureDef.isSensor = fixture["is sensor"].asBool();
			const Json::Value roles{value["roles"]};
			for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
			{
				if(roles[j].asString() == "foot sensor")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Foot));
				else if(roles[j].asString() == "main")
					//Add the role to the data
					fixtureDef.userData = add(fixtureDef.userData, static_cast<long unsigned int>(FixtureRole::Main));
			}
			component->body->CreateFixture(&fixtureDef);
		}
	}
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component)
{
	const std::string identifier{value["identifier"].asString()};
	m_context.textureManager.load(identifier, paths[m_context.parameters.scaleIndex] + "/" + identifier);
	component->sprite.setTexture(m_context.textureManager.get(identifier));
	if(value.isMember("origin"))
		component->sprite.setOrigin(value["origin"]["x"].asFloat()*m_context.parameters.scale, value["origin"]["y"].asFloat()*m_context.parameters.scale);
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component)
{
	component->transform.x = value["x"].asFloat();
	component->transform.y = value["y"].asFloat();
	component->transform.z = value["z"].asFloat();
	component->transform.angle = value["angle"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component, const std::map<std::string, entityx::Entity>& m_entitiesMap)
{
	component->items.clear();
	for(Json::ArrayIndex i{0}; i < value["items"].size(); ++i)
		if(m_entitiesMap.find(value["items"][i].asString()) != m_entitiesMap.end())
			component->items.push_back(m_entitiesMap.at(value["items"][i].asString()));
	component->maxWeight = value["maximum weight"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::ComponentHandle<SpriteComponent> spriteComponent)
{
	component->animationsManager.deserialize(value, spriteComponent->sprite, m_context);
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component)
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

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component)
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

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component)
{
	component->fallingResistance = value["falling resistance"].asFloat();
	component->contactCount = 0;
	component->inAir = true;
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component)
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

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component)
{
	component->jumpStrength = value["strength"].asFloat();
	component->mustJump = false;
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<ArcherComponent> component, const std::map<std::string, entityx::Entity>& m_entitiesMap)
{
	component->damages = value["damages"].asFloat();
	component->initialSpeed = value["initial speed"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<ArrowComponent> component, const std::map<std::string, entityx::Entity>& m_entitiesMap)
{
	component->friction = value["friction"].asFloat();
	component->penetrance = value["penetrance"].asFloat();
	component->damage = value["damage"].asFloat();
	component->localFrictionPoint.x = value["local friction point"]["x"].asFloat();
	component->localFrictionPoint.y = value["local friction point"]["y"].asFloat();
	component->localStickPoint.x = value["local stick point"]["x"].asFloat();
	component->localStickPoint.y = value["local stick point"]["y"].asFloat();
	if(value.isMember("shooter") and m_entitiesMap.find(value["shooter"].asString()) != m_entitiesMap.end())
		component->shooter = m_entitiesMap.at(value["shooter"].asString());
	if(value["state"] == "fired")
		component->state = ArrowComponent::Fired;
	else if(value["state"] == "sticked")
		component->state = ArrowComponent::Sticked;
	else if(value["state"] == "stored")
		component->state = ArrowComponent::Stored;
	else if(value["state"] == "notched")
		component->state = ArrowComponent::Notched;
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<HardnessComponent> component)
{
	component->hardness = value.asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component)
{
	for(Json::ArrayIndex i{0}; i < value["scripts"].size(); ++i)
	{
		std::string scriptName{value["scripts"][i].asString()};
		m_context.scriptManager.load(scriptName, "resources/scripts/" + scriptName);
        component->scriptsNames.push_back(scriptName);
	}
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<DetectionRangeComponent> component)
{
	component->detectionRange = value.asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<DeathComponent> component)
{
	component->dead = value["dead"].asBool();
	for(Json::ArrayIndex i{0}; i < value["drops"].size(); ++i)
		component->drops.push_back({value["drops"][i]["category"].asString(),
									value["drops"][i]["type"].asString(),
									value["drops"][i]["probability"].asFloat(),
									value["drops"][i]["max drops"].asUInt()});
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<NameComponent> component)
{
	component->name = value.asString();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<HandToHandComponent> component)
{
	component->damages = value["damages"].asFloat();
	component->delay = sf::seconds(value["delay"].asFloat());
	component->lastShoot = sf::seconds(value["last shoot"].asFloat());
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<ActorComponent> component)
{
	component->handToHandResistance = value["hand to hand resistance"].asFloat();
	component->magicResistance = value["magic resistance"].asFloat();
	component->arrowResistance = value["arrow resistance"].asFloat();
	component->fireResistance = value["fire resistance"].asFloat();
	component->frostResistance = value["frost resistance"].asFloat();
	component->poisonResistance = value["poison resistance"].asFloat();
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<HoldItemComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent)
{
	component->item = m_entitiesMap.at(value["item"].asString());
	const b2Vec2 localAnchor(value["local anchor"]["x"].asFloat(), value["local anchor"]["y"].asFloat());
	b2WeldJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = component->item.component<BodyComponent>->body;
	jointDef.localAnchorA = localAnchor*(1.f/m_context.parameters.pixelByMeter);
	jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(jointDef.bodyA->GetWorldPoint(jointDef.localAnchorA));
	jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
	jointDef.frequencyHz = 0.f;
	jointDef.dampingRatio = 0.f;
	component->joint = m_context.world.CreateJoint(&jointDef);
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<ArticuledArmsComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent)
{
	component->arms = m_entitiesMap.at(value["arms"].asString());
	component->targetAngle = value["target angle"].asFloat() * b2_pi / 180.f;
	const b2Vec2 localAnchor(value["local anchor"]["x"].asFloat(), value["local anchor"]["y"].asFloat());
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = component->arms.component<BodyComponent>->body;
	jointDef.localAnchorA = localAnchor*(1.f/m_context.parameters.pixelByMeter);
	jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(jointDef.bodyA->GetWorldPoint(jointDef.localAnchorA));
	jointDef.lowerAngle = value["lower angle"].asFloat() * b2_pi / 180.f;
	jointDef.upperAngle = value["upper angle"].asFloat() * b2_pi / 180.f;
	jointDef.referenceAngle = value["current angle"].asFloat() * b2_pi / 180.f;
	jointDef.enableLimit = true;
	jointDef.maxMotorTorque = 10.f;
	jointDef.motorSpeed = 0.f;
	jointDef.enableMotor = true;
	component->armsJoint = m_context.world.CreateJoint(&jointDef);
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent)
{
	component->notchedArrow = m_entitiesMap.at(value["notched arrow"].asString());
	component->targetTranslation = value["target translation"].asFloat()/m_context.parameters.pixelByMeter;
	const b2Vec2 localAnchor(value["local anchor"]["x"].asFloat(), value["local anchor"]["y"].asFloat());

	b2PrismaticJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = component->arms.component<BodyComponent>->body;
	jointDef.localAnchorA = localAnchor*(1.f/m_context.parameters.pixelByMeter);
	jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(jointDef.bodyA->GetWorldPoint(jointDef.localAnchorA));
	jointDef.localAxisA = {1.f, 0.f};
	jointDef.lowerTranslation = value["lower translation"].asFloat()/pixelByMeter;
	jointDef.upperTranslation = value["upper translation"].asFloat()/pixelByMeter;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 10.f;
	jointDef.motorSpeed = 0.f;
	jointDef.enableMotor = true;
	jointDef.referenceAngle = 0.f;
}

void Serializer::deserialize(const Json::Value& value, entityx::ComponentHandle<QuiverComponent> component, entityx::ComponentHandle<BodyComponent> bodyComponent)
{
	component->arrows.clear();
	for(Json::ArrayIndex i{0}; i < value["arrows"].size(); ++i)
	{
		if(m_entitiesMap.find(value["arrows"][i].asString()) != m_entitiesMap.end())
		{
			entityx::Entity arrow{m_entitiesMap.at(value["arrows"][i].asString())};
			component->arrows.push_back(arrow);
			b2WeldJointDef jointDef;
			jointDef.bodyA = bodyComponent->body;
			jointDef.bodyB = arrow.component<BodyComponent>->body;
			jointDef.localAnchorA = b2Vec2(0, 0)*(1.f/m_context.parameters.pixelByMeter);
			jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(jointDef.bodyA->GetWorldPoint(jointDef.localAnchorA));
			jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
			jointDef.frequencyHz = 0.f;
			jointDef.dampingRatio = 0.f;
			m_context.world.CreateJoint(&jointDef);
		}
	}
	component->capacity = value["quiver capacity"].asUInt();
}

//End deserialize

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
