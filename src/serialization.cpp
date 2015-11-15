#include <dist/json/json.h>
#include <entityx/Entity.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
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

Serializer::Serializer(StateStack::Context context, std::map<std::string, entityx::Entity>& entities, Json::Value& jsonEntities):
	m_context(context),
	m_entities(entities),
	m_jsonEntities(jsonEntities)
{
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<BodyComponent> component)
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
	ret["angular velocity"] = component->body->GetAngularVelocity()*180.f/b2_pi;
	ret["linear damping"] = component->body->GetLinearDamping()*pixelByMeter;
	ret["angular damping"] = component->body->GetAngularDamping()*180.f/b2_pi;
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
			default:
				break;
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<SpriteComponent> component)
{
	Json::Value ret;
	const sf::Texture* tex{component->sprite.getTexture()};//Get the associated texture
	ret["identifier"] = m_context.textureManager.getIdentifier(*tex);//Get the identifier of the texture
	if(component->sprite.getOrigin() != sf::Vector2f(0, 0))
	{
		ret["origin"]["x"] = component->sprite.getOrigin().x/m_context.parameters.scale;
		ret["origin"]["y"] = component->sprite.getOrigin().y/m_context.parameters.scale;
	}
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<TransformComponent> component)
{
	Json::Value ret;
	ret["x"] = component->transform.x;
	ret["y"] = component->transform.y;
	ret["z"] = component->transform.z;
	ret["angle"] = component->transform.angle;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<InventoryComponent> component)
{
	Json::Value ret;
	ret["maximum weight"] = component->maxWeight;
	ret["items"] = Json::Value(Json::arrayValue);
	//For each item of the inventory
	for(entityx::Entity item : component->items)
	{
		if(not TEST(isMember(m_entities, item) and item.valid()))
			return Json::Value();
		//Add it to the value
		ret["items"].append(getKey(m_entities, item));
	}
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component)
{
	return component->animationsManager.serialize();
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<DirectionComponent> component)
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<CategoryComponent> component)
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<FallComponent> component)
{
	Json::Value ret;
	ret["falling resistance"] = component->fallingResistance;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<WalkComponent> component)
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<JumpComponent> component)
{
	Json::Value ret;
	ret["strength"] = component->jumpStrength;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ArcherComponent> component)
{
	Json::Value ret;
	ret["damages"] = component->damages;
	ret["initial speed"] = component->initialSpeed;
	ret["quiver"] = getKey(m_entities, component->quiver);
	ret["quiver anchor"]["x"] = component->quiverAnchor.x*m_context.parameters.pixelByMeter;
	ret["quiver anchor"]["y"] = component->quiverAnchor.y*m_context.parameters.pixelByMeter;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<HealthComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<StaminaComponent> component)
{
	Json::Value ret;
	ret["current"] = component->current;
	ret["maximum"] = component->maximum;
	ret["regeneration"] = component->regeneration;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ArrowComponent> component)
{
	Json::Value ret;
	ret["friction"] = component->friction;
	ret["penetrance"] = component->penetrance;
	ret["damage"] = component->damage;
	ret["local friction point"]["x"] = component->localFrictionPoint.x;
	ret["local friction point"]["y"] = component->localFrictionPoint.y;
	ret["local stick point"]["x"] = component->localStickPoint.x;
	ret["local stick point"]["y"] = component->localStickPoint.y;
	if(component->shooter)
	{
		if(not TEST(isMember(m_entities, component->shooter)))
			return Json::Value();
		ret["shooter"] = getKey(m_entities, component->shooter);
	}
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<HardnessComponent> component)
{
	return component->hardness;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ScriptsComponent> component)
{
	Json::Value ret;
	ret["scripts"] = Json::Value(Json::arrayValue);
	for(auto& scriptName : component->scriptsNames)
        ret["scripts"].append(scriptName);
    return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<DetectionRangeComponent> component)
{
	return component->detectionRange;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<DeathComponent> component)
{
	Json::Value ret;
	ret["dead"] = component->dead;
	ret["drops"] = Json::arrayValue;
	for(auto& drop : component->drops)
	{
		Json::Value dropJson;
		dropJson["category"] = drop.category;
		dropJson["type"] = drop.type;
		dropJson["weight"] = drop.weight;
		dropJson["value"] = drop.value;
		dropJson["probability"] = drop.probability;
		dropJson["max drops"] = drop.maxDrops;
		ret["drops"].append(dropJson);
	}
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<NameComponent> component)
{
	return component->name;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<HandToHandComponent> component)
{
	Json::Value ret;
	ret["damages"] = component->damages;
	ret["delay"] = component->delay.asSeconds();
	ret["last shoot"] = component->lastShoot.asSeconds();
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ActorComponent> component)
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

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ItemComponent> component)
{
	Json::Value ret;
	ret["type"] = component->type;
	ret["category"] = component->category;
	ret["weight"] = component->weight;
	ret["value"] = component->value;
	ret["hold anchor"]["x"] = component->holdAnchor.x;
	ret["hold anchor"]["y"] = component->holdAnchor.y;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<HoldItemComponent> component)
{
	Json::Value ret;
	ret["body anchor"]["x"] = m_context.parameters.pixelByMeter * component->bodyAnchor.x;
	ret["body anchor"]["y"] = m_context.parameters.pixelByMeter * component->bodyAnchor.y;
	if(not TEST(isMember(m_entities, component->item)))
		return Json::Value();
	ret["item"] = getKey(m_entities, component->item);
	ret["item anchor"]["x"] = m_context.parameters.pixelByMeter * component->itemAnchor.x;
	ret["item anchor"]["y"] = m_context.parameters.pixelByMeter * component->itemAnchor.y;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<ArticuledArmsComponent> component)
{
	Json::Value ret;
	ret["target angle"] = component->targetAngle * 180.f / b2_pi;
	ret["upper angle"] = component->upperAngle * 180.f / b2_pi;
	ret["lower angle"] = component->lowerAngle * 180.f / b2_pi;
	ret["current angle"] = component->armsJoint->GetJointAngle() * 180.f / b2_pi;
	if(not TEST(isMember(m_entities, component->arms)))
		return Json::Value();
	ret["arms"] = getKey(m_entities, component->arms);
	ret["arms anchor"]["x"] = component->armsAnchor.x * m_context.parameters.pixelByMeter;
	ret["arms anchor"]["y"] = component->armsAnchor.y * m_context.parameters.pixelByMeter;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<BowComponent> component)
{
	Json::Value ret;
	ret["upper translation"] = component->upperTranslation * m_context.parameters.pixelByMeter;
	ret["lower translation"] = component->lowerTranslation * m_context.parameters.pixelByMeter;
	ret["target translation"] = component->targetTranslation * m_context.parameters.pixelByMeter;
	ret["current translation"] = component->notchedArrowJoint->GetJointTranslation() * m_context.parameters.pixelByMeter;
	if(component->notchedArrow.valid())
	{
		if(not TEST(isMember(m_entities, component->notchedArrow)))
			return Json::Value();
		ret["notched arrow"] = getKey(m_entities, component->notchedArrow);
	}
	ret["notched arrow anchor"]["x"] = component->notchedArrowAnchor.x * m_context.parameters.pixelByMeter;
	ret["notched arrow anchor"]["y"] = component->notchedArrowAnchor.y * m_context.parameters.pixelByMeter;
	return ret;
}

Json::Value Serializer::implSerialize(entityx::ComponentHandle<QuiverComponent> component)
{
	Json::Value ret;
	ret["capacity"] = component->capacity;
	ret["arrows"] = Json::Value(Json::arrayValue);
	//For each arrow of the quiver
	for(auto& pair : component->arrows)
	{
		if(not TEST(pair.first.valid() and isMember(m_entities, pair.first)))
			return Json::Value();
		//Add it to the value
		ret["arrows"].append(getKey(m_entities, pair.first));
	}
	ret["arrow anchor"]["x"] = component->arrowAnchor.x * m_context.parameters.pixelByMeter;
	ret["arrow anchor"]["y"] = component->arrowAnchor.y * m_context.parameters.pixelByMeter;
	ret["body anchor"]["x"] = component->bodyAnchor.x * m_context.parameters.pixelByMeter;
	ret["body anchor"]["y"] = component->bodyAnchor.y * m_context.parameters.pixelByMeter;
	return ret;
}

//End serialize

  //////////               //////////
 ////////// Deserializing //////////
//////////               //////////

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<BodyComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<TransformComponent> transformComponent{entity.component<TransformComponent>()};
	if(not TEST(transformComponent))
		throw std::runtime_error("Transform component lacks.");
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

	bodyDef.position.x = transformComponent->transform.x/pixelByMeter;
	bodyDef.position.y = transformComponent->transform.y/pixelByMeter;
	bodyDef.angle = transformComponent->transform.angle*b2_pi/180.f;
	bodyDef.linearVelocity.x = value["linear velocity"]["x"].asFloat()/pixelByMeter;
	bodyDef.linearVelocity.y = value["linear velocity"]["y"].asFloat()/pixelByMeter;
	bodyDef.angularVelocity = value["angular velocity"].asFloat()*b2_pi/180.f;
	bodyDef.linearDamping = value["linear damping"].asFloat()/pixelByMeter;
	bodyDef.angularDamping = value["angular damping"].asFloat()*b2_pi/180.f;
	bodyDef.allowSleep = value["allow sleep"].asBool();
	bodyDef.awake = value["awake"].asBool();
	bodyDef.fixedRotation = value["fixed rotation"].asBool();
	bodyDef.bullet = value["bullet"].asBool();
	bodyDef.active = value["active"].asBool();
	bodyDef.gravityScale = value["gravity scale"].asFloat();

	component->body = m_context.world.CreateBody(&bodyDef);

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

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<SpriteComponent> component, entityx::Entity)
{
	const std::string identifier{value["identifier"].asString()};
	m_context.textureManager.load(identifier, paths[m_context.parameters.scaleIndex] + "/" + identifier);
	component->sprite.setTexture(m_context.textureManager.get(identifier));
	if(value.isMember("origin"))
		component->sprite.setOrigin(value["origin"]["x"].asFloat()*m_context.parameters.scale, value["origin"]["y"].asFloat()*m_context.parameters.scale);
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<TransformComponent> component, entityx::Entity)
{
	component->transform.x = value["x"].asFloat();
	component->transform.y = value["y"].asFloat();
	component->transform.z = value["z"].asFloat();
	component->transform.angle = value["angle"].asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<InventoryComponent> component, entityx::Entity)
{
	component->items.clear();
	for(Json::ArrayIndex i{0}; i < value["items"].size(); ++i)
		if(m_entities.find(value["items"][i].asString()) != m_entities.end())
			component->items.push_back(m_entities.at(value["items"][i].asString()));
	component->maxWeight = value["maximum weight"].asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<AnimationsComponent<SpriteSheetAnimation>> component, entityx::Entity entity)
{
	entityx::ComponentHandle<SpriteComponent> spriteComponent{entity.component<SpriteComponent>()};
	if(not TEST(spriteComponent))
		throw std::runtime_error("Sprite component lacks.");
	component->animationsManager.deserialize(value, spriteComponent->sprite, m_context);
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<DirectionComponent> component, entityx::Entity)
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

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<CategoryComponent> component, entityx::Entity)
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

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<FallComponent> component, entityx::Entity)
{
	component->fallingResistance = value["falling resistance"].asFloat();
	component->contactCount = 0;
	component->inAir = true;
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<WalkComponent> component, entityx::Entity)
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

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<JumpComponent> component, entityx::Entity)
{
	component->jumpStrength = value["strength"].asFloat();
	component->mustJump = false;
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArcherComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<BodyComponent> bodyComponent{entity.component<BodyComponent>()};
	if(not TEST(bodyComponent))
		throw std::runtime_error("Body component lacks.");
	component->damages = value["damages"].asFloat();
	component->initialSpeed = value["initial speed"].asFloat();
	component->quiver = m_entities.at(value["quiver"].asString());
	component->quiverAnchor.x = value["quiver anchor"]["x"].asFloat()/m_context.parameters.pixelByMeter;
	component->quiverAnchor.y = value["quiver anchor"]["y"].asFloat()/m_context.parameters.pixelByMeter;
	if(not TEST(component->quiver))
		throw std::runtime_error("Quiver entity is invalid.");
	const QuiverComponent::Handle quiverComponent(component->quiver.component<QuiverComponent>());
	const BodyComponent::Handle quiverBodyComponent(component->quiver.component<BodyComponent>());
	if(not TEST(quiverComponent and quiverBodyComponent))
		throw std::runtime_error("Quiver or body component lacks.");
	b2WeldJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = quiverBodyComponent->body;
	jointDef.localAnchorA = component->quiverAnchor;
	jointDef.localAnchorB = quiverComponent->bodyAnchor;
	jointDef.referenceAngle = 0.f;
	jointDef.frequencyHz = 0.f;
	jointDef.dampingRatio = 0.f;
	component->quiverJoint = static_cast<b2WeldJoint*>(m_context.world.CreateJoint(&jointDef));
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<HealthComponent> component, entityx::Entity)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<StaminaComponent> component, entityx::Entity)
{
	component->current = value["current"].asFloat();
	component->maximum = value["maximum"].asFloat();
	component->regeneration = value["regeneration"].asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArrowComponent> component, entityx::Entity)
{
	component->friction = value["friction"].asFloat();
	component->penetrance = value["penetrance"].asFloat();
	component->damage = value["damage"].asFloat();
	component->localFrictionPoint.x = value["local friction point"]["x"].asFloat();
	component->localFrictionPoint.y = value["local friction point"]["y"].asFloat();
	component->localStickPoint.x = value["local stick point"]["x"].asFloat();
	component->localStickPoint.y = value["local stick point"]["y"].asFloat();
	if(value.isMember("shooter"))
	{
		if(not TEST(m_entities.find(value["shooter"].asString()) != m_entities.end()))
			throw std::runtime_error("Unknow shooter entity.");
		component->shooter = m_entities.at(value["shooter"].asString());
	}
	if(value["state"] == "fired")
		component->state = ArrowComponent::Fired;
	else if(value["state"] == "sticked")
		component->state = ArrowComponent::Sticked;
	else if(value["state"] == "stored")
		component->state = ArrowComponent::Stored;
	else if(value["state"] == "notched")
		component->state = ArrowComponent::Notched;
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<HardnessComponent> component, entityx::Entity)
{
	component->hardness = value.asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ScriptsComponent> component, entityx::Entity)
{
	for(Json::ArrayIndex i{0}; i < value["scripts"].size(); ++i)
	{
		std::string scriptName{value["scripts"][i].asString()};
		m_context.scriptManager.load(scriptName, "resources/scripts/" + scriptName);
        component->scriptsNames.push_back(scriptName);
	}
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<DetectionRangeComponent> component, entityx::Entity)
{
	component->detectionRange = value.asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<DeathComponent> component, entityx::Entity)
{
	component->dead = value["dead"].asBool();
	for(Json::ArrayIndex i{0}; i < value["drops"].size(); ++i)
		component->drops.push_back({value["drops"][i]["category"].asString(),
				value["drops"][i]["type"].asString(),
				value["drops"][i]["probability"].asFloat(),
				value["drops"][i]["weight"].asFloat(),
				value["drops"][i]["value"].asFloat(),
				value["drops"][i]["max drops"].asUInt()});
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<NameComponent> component, entityx::Entity)
{
	component->name = value.asString();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<HandToHandComponent> component, entityx::Entity)
{
	component->damages = value["damages"].asFloat();
	component->delay = sf::seconds(value["delay"].asFloat());
	component->lastShoot = sf::seconds(value["last shoot"].asFloat());
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ActorComponent> component, entityx::Entity)
{
	component->handToHandResistance = value["hand to hand resistance"].asFloat();
	component->magicResistance = value["magic resistance"].asFloat();
	component->arrowResistance = value["arrow resistance"].asFloat();
	component->fireResistance = value["fire resistance"].asFloat();
	component->frostResistance = value["frost resistance"].asFloat();
	component->poisonResistance = value["poison resistance"].asFloat();
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ItemComponent> component, entityx::Entity)
{
	component->type = value["type"].asString();
	component->category = value["category"].asString();
	component->weight = value["weight"].asFloat();
	component->value = value["value"].asFloat();
	component->holdAnchor.x = value["hold anchor"]["x"].asFloat()/m_context.parameters.pixelByMeter;
	component->holdAnchor.y = value["hold anchor"]["y"].asFloat()/m_context.parameters.pixelByMeter;
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<ArticuledArmsComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<BodyComponent> bodyComponent{entity.component<BodyComponent>()};
	if(not TEST(bodyComponent))
		throw std::runtime_error("Body component lacks.");
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	component->arms = m_entities.at(value["arms"].asString());
	component->targetAngle = value["target angle"].asFloat() * b2_pi / 180.f;
	component->lowerAngle = value["lower angle"].asFloat() * b2_pi / 180.f;
	component->upperAngle = value["upper angle"].asFloat() * b2_pi / 180.f;
	component->armsAnchor.x = value["arms anchor"]["x"].asFloat()/pixelByMeter;
	component->armsAnchor.y = value["arms anchor"]["y"].asFloat()/pixelByMeter;
	if(not TEST(component->arms))
		throw std::runtime_error("Invalid arms entity.");
	const BodyComponent::Handle armsBodyComponent{component->arms.component<BodyComponent>()};
	const HoldItemComponent::Handle holdItemComponent{component->arms.component<HoldItemComponent>()};
	if(not TEST(armsBodyComponent and holdItemComponent))
		throw std::runtime_error("Body or hold item component of arms lacks.");
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = armsBodyComponent->body;
	jointDef.localAnchorA = component->armsAnchor;
	jointDef.localAnchorB = holdItemComponent->bodyAnchor;
	jointDef.lowerAngle = component->lowerAngle;
	jointDef.upperAngle = component->upperAngle;
	jointDef.referenceAngle = value["current angle"].asFloat() * b2_pi / 180.f;
	jointDef.enableLimit = false;
	jointDef.maxMotorTorque = 10.f;
	jointDef.motorSpeed = 0.f;
	jointDef.enableMotor = true;
	component->armsJoint = static_cast<b2RevoluteJoint*>(m_context.world.CreateJoint(&jointDef));
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<HoldItemComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<BodyComponent> bodyComponent{entity.component<BodyComponent>()};
	if(not TEST(bodyComponent))
		throw std::runtime_error("Body component lacks.");
	const float pixelByMeter{m_context.parameters.pixelByMeter};
	component->item = m_entities.at(value["item"].asString());
	component->bodyAnchor.x = value["body anchor"]["x"].asFloat()/pixelByMeter;
	component->bodyAnchor.y = value["body anchor"]["y"].asFloat()/pixelByMeter;
	component->itemAnchor.x = value["item anchor"]["x"].asFloat()/pixelByMeter;
	component->itemAnchor.x = value["item anchor"]["y"].asFloat()/pixelByMeter;
	const BodyComponent::Handle itemBodyComponent{component->item.component<BodyComponent>()};
	const ItemComponent::Handle itemComponent{component->item.component<ItemComponent>()};
	if(not TEST(itemBodyComponent and itemComponent))
		throw std::runtime_error("Body or item component of item lacks.");
	b2WeldJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = itemBodyComponent->body;
	jointDef.localAnchorA = component->itemAnchor;
	jointDef.localAnchorB = itemComponent->holdAnchor;
	jointDef.referenceAngle = 0.f;
	jointDef.frequencyHz = 0.f;
	jointDef.dampingRatio = 0.f;
	component->itemJoint = static_cast<b2WeldJoint*>(m_context.world.CreateJoint(&jointDef));
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<BowComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<BodyComponent> bodyComponent{entity.component<BodyComponent>()};
	if(not TEST(bodyComponent))
		throw std::runtime_error("Body component lacks.");
	component->notchedArrow = m_entities.at(value["notched arrow"].asString());
	component->targetTranslation = value["target translation"].asFloat();
	component->lowerTranslation = value["lower translation"].asFloat()/m_context.parameters.pixelByMeter;
	component->upperTranslation = value["upper translation"].asFloat()/m_context.parameters.pixelByMeter;
	component->notchedArrowAnchor.x = value["notched arrow anchor"]["x"].asFloat()/m_context.parameters.pixelByMeter;
	component->notchedArrowAnchor.x = value["notched arrow anchor"]["y"].asFloat()/m_context.parameters.pixelByMeter;
	const BodyComponent::Handle notchedArrowBodyComponent{component->notchedArrow.component<BodyComponent>()};
	ArrowComponent::Handle arrowComponent{component->notchedArrow.component<ArrowComponent>()};
	if(not TEST(notchedArrowBodyComponent and arrowComponent))
		throw std::runtime_error("Body or arrow component of notched arrow lacks.");
	b2PrismaticJointDef jointDef;
	jointDef.bodyA = bodyComponent->body;
	jointDef.bodyB = component->notchedArrow.component<BodyComponent>()->body;
	jointDef.localAnchorA = component->notchedArrowAnchor;
	jointDef.localAnchorB = sftob2(arrowComponent->localFrictionPoint/m_context.parameters.pixelByMeter);
	jointDef.localAxisA = {1.f, 0.f};
	jointDef.lowerTranslation = component->lowerTranslation;
	jointDef.upperTranslation = component->upperTranslation;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 10.f;
	jointDef.motorSpeed = 0.f;
	jointDef.enableMotor = true;
	jointDef.referenceAngle = -b2_pi/2.f;
	component->notchedArrowJoint = static_cast<b2PrismaticJoint*>(m_context.world.CreateJoint(&jointDef));
	arrowComponent->state = ArrowComponent::Notched;
}

void Serializer::implDeserialize(const Json::Value& value, entityx::ComponentHandle<QuiverComponent> component, entityx::Entity entity)
{
	const entityx::ComponentHandle<BodyComponent> bodyComponent{entity.component<BodyComponent>()};
	if(not TEST(bodyComponent))
		throw std::runtime_error("Body component lacks.");
	component->arrows.clear();
	for(Json::ArrayIndex i{0}; i < value["arrows"].size(); ++i)
	{
		if(m_entities.find(value["arrows"][i].asString()) != m_entities.end())
		{
			entityx::Entity arrow{m_entities.at(value["arrows"][i].asString())};
			if(not TEST(arrow))
				throw std::runtime_error("An arrow in a quiver is not valid.");
			const BodyComponent::Handle arrowBodyComponent{arrow.component<BodyComponent>()};
			const ArrowComponent::Handle arrowComponent{arrow.component<ArrowComponent>()};
			if(not TEST(arrowBodyComponent and arrowComponent))
				throw std::runtime_error("Arrow or body component of arrow in a quiver lacks.");
			b2WeldJointDef jointDef;
			jointDef.bodyA = bodyComponent->body;
			jointDef.bodyB = arrowBodyComponent->body;
			jointDef.localAnchorA = component->arrowAnchor;
			jointDef.localAnchorB = sftob2(arrowComponent->localStickPoint/m_context.parameters.pixelByMeter);
			jointDef.referenceAngle = -b2_pi/2.f;
			jointDef.frequencyHz = 0.f;
			jointDef.dampingRatio = 0.f;
			component->arrows.emplace(arrow, static_cast<b2WeldJoint*>(m_context.world.CreateJoint(&jointDef)));
		}
	}
	component->capacity = value["capacity"].asUInt();
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
