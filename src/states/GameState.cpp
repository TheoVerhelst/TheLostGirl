#include <iostream>
#include <fstream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <dist/json/json.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/serialization.h>

#include <TheLostGirl/states/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_levelEntities(),
	m_contactListener(getContext()),
	m_timeSpeed{1.f},
	m_threadLoad(),
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 0, 1080}
{
	//Dunno how to make a cleaner thread initialization from function member
	m_threadLoad = std::thread([this](const std::string& str){return this->initWorld(str);}, "resources/levels/save.json");
}

GameState::~GameState()
{
	for(auto& entity : m_entities)
	{
		if(entity.second.has_component<BodyComponent>())
		{
			for(auto& partBody : entity.second.component<BodyComponent>()->bodies)
				getContext().world.DestroyBody(partBody.second);
		}
		entity.second.destroy();
	}
	getContext().world.ClearForces();
	if(m_threadLoad.joinable())
		m_threadLoad.join();
}

void GameState::draw()
{
	getContext().systemManager.update<RenderSystem>(sf::Time::Zero.asSeconds());
	//The drag and drop system draw a line on the screen, so we must put it here
	getContext().systemManager.update<DragAndDropSystem>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<PhysicsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ActionsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ScrollingSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	getContext().systemManager.update<SkySystem>(getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds());
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);
	
	getContext().player.handleEvent(event, getContext().commandQueue);
	//Update the drag and drop state
	bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);
	return false;
}

void GameState::saveWorld(const std::string& file)
{
		Json::Value root;//Will contains the root value after serializing.
		Json::StyledWriter writer;
		std::ofstream saveFileStream(file, std::ofstream::binary);
		for(auto& entity : m_entities)
		{
			if(entity.second.has_component<BodyComponent>())
				root["entities"][entity.first]["body"] = serialize(entity.second.component<BodyComponent>());
			
			if(entity.second.has_component<SpriteComponent>())
				root["entities"][entity.first]["sprite"] = serialize(entity.second.component<SpriteComponent>(), getContext().textureManager);
			
			if(entity.second.has_component<AnimationsComponent<SpriteSheetAnimation>>())
				root["entities"][entity.first]["spritesheet animations"] = serialize(entity.second.component<AnimationsComponent<SpriteSheetAnimation>>());
			
			if(entity.second.has_component<DirectionComponent>())
				root["entities"][entity.first]["direction"] = serialize(entity.second.component<DirectionComponent>());
			
			if(entity.second.has_component<CategoryComponent>())
				root["entities"][entity.first]["category"] = serialize(entity.second.component<CategoryComponent>());
			
			if(entity.second.has_component<ActorIDComponent>())
				root["entities"][entity.first]["actor ID"] = serialize(entity.second.component<ActorIDComponent>());
			
			if(entity.second.has_component<SkyComponent>())
				root["entities"][entity.first]["sky"] = serialize(entity.second.component<SkyComponent>());
			
			if(entity.second.has_component<FallComponent>())
				root["entities"][entity.first]["fall"] = serialize(entity.second.component<FallComponent>());
			
			if(entity.second.has_component<WalkComponent>())
				root["entities"][entity.first]["walk"] = serialize(entity.second.component<WalkComponent>());
			
			if(entity.second.has_component<JumpComponent>())
				root["entities"][entity.first]["jump"] = serialize(entity.second.component<JumpComponent>());
			
			if(entity.second.has_component<BendComponent>())
				root["entities"][entity.first]["bend"] = serialize(entity.second.component<BendComponent>());
			
			if(entity.second.has_component<HealthComponent>())
				root["entities"][entity.first]["health"] = serialize(entity.second.component<HealthComponent>());
			
			if(entity.second.has_component<StaminaComponent>())
				root["entities"][entity.first]["stamina"] = serialize(entity.second.component<StaminaComponent>());
		}
		
		//level data
		root["level"]["identifier"] = m_levelIdentifier;
		root["level"]["box"]["x"] = m_levelRect.left;
		root["level"]["box"]["y"] = m_levelRect.top;
		root["level"]["box"]["w"] = m_levelRect.width;
		root["level"]["box"]["h"] = m_levelRect.height;
		root["level"]["number of plans"] = m_numberOfPlans;
		root["level"]["reference plan"] = m_referencePlan;
		
		saveFileStream << writer.write(root);
		saveFileStream.close();
}

void GameState::initWorld(const std::string& file)
{
	const float scale = getContext().parameters.scale;
	const float pixelScale = getContext().parameters.pixelScale;
	const float uniqScale = scale / pixelScale;//The pixel/meters scale at the maximum resolution, about 1.f/120.f
	TextureManager& texManager = getContext().textureManager;
	getContext().eventManager.emit<LoadingStateChange>(0, LangManager::tr("Loading save file"));
	try
	{
		//Parse the level data
		Json::Value root;//Will contains the root value after parsing.
		Json::Reader reader;
		std::ifstream saveFileStream(file, std::ifstream::binary);
		if(!reader.parse(saveFileStream, root))//report to the user the failure and their locations in the document.
			throw std::runtime_error(reader.getFormattedErrorMessages());
		
		//Assert that there is only these elements with these types in the root object.
		parseObject(root, "root", {{"entities", Json::objectValue},
									{"joints", Json::arrayValue},
									{"level", Json::objectValue},
									{"time", Json::objectValue}});
		requireValues(root, "root", {{"level", Json::objectValue}});
		
		//time
		if(root.isMember("time"))
		{
			const Json::Value time = root["time"];
			parseObject(time, "time", {{"date", Json::realValue}, {"speed", Json::realValue}});
			
			//date
			if(time.isMember("date"))
				getContext().systemManager.system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
			
			//speed
			if(time.isMember("speed"))
				m_timeSpeed = time["speed"].asFloat();
		}
		if(root.isMember("level"))
		{
			const Json::Value level = root["level"];
			
			//number of plans
			//must load it now in order to now how much plans can be defined
			if(level.isMember("number of plans"))
				m_numberOfPlans = level["number of plans"].asUInt();
			
			//Generate a map containing all the plan numbers and the type of each
			std::map<std::string, Json::ValueType> plans;
			for(unsigned short int i{0}; i < m_numberOfPlans; i++)
				plans[std::to_string(i)] = Json::arrayValue;
			
			//add the others values
			plans["number of plans"] = Json::intValue;
			plans["reference plan"] = Json::realValue;
			plans["box"] = Json::objectValue;
			plans["identifier"] = Json::stringValue;
			
			//Requires that all the identifiers in "level" are in the map "plans"
			parseObject(level, "level", plans);
			requireValues(level, "level", {{"box", Json::objectValue}, {"identifier", Json::stringValue}});
			
			//identifier
			std::string identifier = level["identifier"].asString();
			if(hasWhiteSpace(identifier))
				throw std::runtime_error("\"level.identifier\" value contains whitespaces.");
			m_levelIdentifier = identifier;
			
			//reference plan
			if(level.isMember("reference plan"))
				m_referencePlan = level["reference plan"].asFloat();
			
			//length
			const Json::Value levelBox = level["box"];
			parseObject(levelBox, "level.box", {{"x", Json::intValue}, {"y", Json::intValue}, {"w", Json::intValue}, {"h", Json::intValue}});
			requireValues(levelBox, "level.box", {{"w", Json::intValue}});
			
			//width
			m_levelRect.width = levelBox["w"].asInt();
			
			//height
			if(levelBox.isMember("h"))
				m_levelRect.height = levelBox["h"].asInt();
			
			//x
			if(levelBox.isMember("x"))
				m_levelRect.top = levelBox["x"].asInt();
			
			//y
			if(levelBox.isMember("y"))
				m_levelRect.left = levelBox["y"].asInt();
			
			//for each plan
			for(unsigned short int i{0}; i < m_numberOfPlans; i++)
			{
				//Filename of the image to load
				std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
				std::cout << "filetexture = " << fileTexture << std::endl;
				//Path of the image to load
				std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				//Check if the plan is defined
				if(level.isMember(std::to_string(i)))
				{
					const Json::Value plan = level[std::to_string(i)];
					parseArray(plan, "level." + std::to_string(i), Json::objectValue);
					//For each image frame in the plan
					for(Json::ArrayIndex j{0}; j < plan.size(); j++)
					{
						const Json::Value image = plan[j];
						requireValues(image, "level." + std::to_string(i) + "." + std::to_string(j), {{"origin", Json::objectValue}, {"replace", Json::arrayValue}});
						const Json::Value origin = image["origin"];
						requireValues(origin, "level." + std::to_string(i) + "." + std::to_string(j) + ".origin", {{"x", Json::intValue},
																										{"y", Json::intValue},
																										{"w", Json::intValue},
																										{"h", Json::intValue}});
						//Coordinates of the original image
						unsigned int ox{static_cast<unsigned int>(origin["x"].asUInt()*getContext().parameters.scale)};
						unsigned int oy{static_cast<unsigned int>(origin["y"].asUInt()*getContext().parameters.scale)};
						unsigned int ow{static_cast<unsigned int>(origin["w"].asUInt()*getContext().parameters.scale)};
						unsigned int oh{static_cast<unsigned int>(origin["h"].asUInt()*getContext().parameters.scale)};
						
						//Load the texture
						//Identifier of the texture, in format "level_plan_texture"
						std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
						//If the texture is not alreday loaded (first loading of the level)
						if(not texManager.isLoaded(textureIdentifier))
						{
							getContext().eventManager.emit<LoadingStateChange>(float(i*100)/float(m_numberOfPlans+1) + (float(j*100)/float(plan.size()*(m_numberOfPlans+1))), LangManager::tr("Loading plan") + L" " + std::to_wstring(i));
							texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(ox, oy, ow, oh));
						}
						
						//Replaces
						const Json::Value replaces = image["replace"];
						parseArray(replaces, "level." + std::to_string(i) + "." + std::to_string(j) + ".replace", Json::objectValue);
						//For each replacing of the image
						for(Json::ArrayIndex k{0}; k < replaces.size(); k++)
						{
							const Json::Value replace = replaces[k];
							//Identifier of the entity, in format "level_plan_texture_replace"
							std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(k)};
							requireValues(replace, "level." + std::to_string(i) + "." + std::to_string(j) + ".replace." + std::to_string(k), {{"x", Json::realValue},
																															{"y", Json::realValue}});
							//Position where the frame should be replaced
							float rx{replace["x"].asFloat()*getContext().parameters.scale};
							float ry{replace["y"].asFloat()*getContext().parameters.scale};
							//Create an entity
							m_levelEntities.emplace(replaceIdentifier, getContext().entityManager.create());
							//Create a sprite with the loaded texture
							//Assign the sprite to the entity
							sf::Sprite replaceSpr(texManager.get(textureIdentifier));
							replaceSpr.setPosition(rx, ry);
							std::map<std::string, sf::Sprite> sprites{{"main", replaceSpr}};
							std::map<std::string, sf::Vector2f> worldPositions{{"main", {rx, ry}}};
							m_levelEntities[replaceIdentifier].assign<SpriteComponent>(sprites, worldPositions, static_cast<float>(i));
							m_levelEntities[replaceIdentifier].assign<CategoryComponent>(Category::Scene);
							std::cout << "assigned to replace " << replaceIdentifier << std::endl;
						}
					}
				}
				//If the plan is not defined, load all the image in multiples chunks
				else
				{
					unsigned int chunkSize{sf::Texture::getMaximumSize()};
					//The length of the plan, relatively to the second.
					unsigned int planLength = (m_levelRect.width * pow(1.5, m_referencePlan - i))*getContext().parameters.scale;
					//Number of chunks to load in this plan
					unsigned int numberOfChunks{(planLength/chunkSize)+1};
					//Path to the image to load
					
					for(unsigned int j{0}; j < numberOfChunks; ++j)
					{
						//Identifier of the entity, in format "level_plan_chunk"
						std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
						//Size of the chunk to load, may be truncated if we reach the end of the image.
						unsigned int currentChunkSize{chunkSize};
						if(j >= planLength/chunkSize)
							currentChunkSize = planLength - chunkSize*j;
						//If the texture is not alreday loaded (first loading of the level)
						if(not texManager.isLoaded(textureIdentifier))
						{
							getContext().eventManager.emit<LoadingStateChange>((float(i*100)/float(m_numberOfPlans+1)) + (float(j*100)/float(numberOfChunks*(m_numberOfPlans+1))), LangManager::tr("Loading plan") + L" " + std::to_wstring(i));
							texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, m_levelRect.height*getContext().parameters.scale));
						}
						//Create an entity
						m_levelEntities.emplace(textureIdentifier, getContext().entityManager.create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite chunkSpr(texManager.get(textureIdentifier));
						chunkSpr.setPosition(j*chunkSize, 0);
						std::map<std::string, sf::Sprite> sprites{{"main", chunkSpr}};
						std::map<std::string, sf::Vector2f> worldPositions{{"main", {static_cast<float>(j*chunkSize), 0}}};
						m_levelEntities[textureIdentifier].assign<SpriteComponent>(sprites, worldPositions, static_cast<float>(i));
						m_levelEntities[textureIdentifier].assign<CategoryComponent>(Category::Scene);
							std::cout << "assigned to normal " << textureIdentifier << std::endl;
					}
				}
			}
		}
		
		//entities
		if(root.isMember("entities"))
		{
			const Json::Value entities = root["entities"];
			//Assert that every element of entities must be an object
			parseObject(entities, "entities", Json::objectValue);
			for(std::string& entityName : entities.getMemberNames())
			{
				const Json::Value entity = entities[entityName];
				//Assert that there is only these elements with these types in the entity object.
				parseObject(entity, "entities." + entityName, {{"categories", Json::arrayValue},
																{"actor ID", Json::intValue},
																{"plan", Json::realValue},
																{"walk", Json::objectValue},
																{"jump", Json::realValue},
																{"bend", Json::objectValue},
																{"health", Json::objectValue},
																{"stamina", Json::objectValue},
																{"direction", Json::objectValue},
																{"fall", Json::objectValue},
																{"parts", Json::objectValue}});
				m_entities.emplace(entityName, getContext().entityManager.create());
				
				//parts
				if(entity.isMember("parts"))
				{
					const Json::Value parts = entity["parts"];
					//Assert that every element of the parts must be an object
					parseObject(parts, "entities." + entityName + ".parts", Json::objectValue);
					for(std::string& partName : parts.getMemberNames())
					{
						const Json::Value part = parts[partName];
						parseObject(part, "entities." + entityName + ".parts." + partName, {{"sprite", Json::objectValue},
																							{"body", Json::objectValue},
																							{"spritesheet animations", Json::objectValue},
																							{"play animations", Json::arrayValue},
																							{"activate animations", Json::arrayValue}});
						
						//sprite
						if(part.isMember("sprite"))
						{
							const Json::Value sprite = part["sprite"];
							parseObject(sprite, "entities." + entityName + ".sprite", {{"identifier", Json::stringValue}, {"position", Json::objectValue}});
							requireValues(sprite, "entities." + entityName + ".sprite", {{"identifier", Json::stringValue}});
							parseValue(sprite["identifier"], "entities." + entityName + ".sprite.identifier", {"archer", "bow", "arms"});
							
							//position
							sf::Vector2f position{0, 0};
							if(sprite.isMember("position"))
							{
								const Json::Value positionObj = sprite["position"];
								parseObject(positionObj, "entities." + entityName + ".body.position", {{"x", Json::realValue}, {"y", Json::realValue}});
								if(positionObj.isMember("x"))
									position.x = positionObj["x"].asFloat()*uniqScale;
								if(positionObj.isMember("y"))
									position.y = positionObj["y"].asFloat()*uniqScale;
							}
							
							//Create the sprite
							sf::Sprite entitySpr(getContext().textureManager.get(sprite["identifier"].asString()));
							//If the component do not already exists
							if(not m_entities[entityName].has_component<SpriteComponent>())
							{
								std::map<std::string, sf::Sprite> sprites{{partName, entitySpr}};
								std::map<std::string, sf::Vector2f> worldPositions{{partName, position}};
								m_entities[entityName].assign<SpriteComponent>(sprites, worldPositions, m_referencePlan);
							}
							else
							{
								m_entities[entityName].component<SpriteComponent>()->sprites.emplace(partName, entitySpr);
								m_entities[entityName].component<SpriteComponent>()->worldPositions.emplace(partName, position);
							}
						}
						
						//body
						if(part.isMember("body"))
						{
							const Json::Value body = part["body"];
							parseObject(body, "entities." + entityName + ".parts." + partName + ".body", {{"type", Json::stringValue},
																					{"position", Json::objectValue},
																					{"angle", Json::realValue},
																					{"linear velocity", Json::objectValue},
																					{"angular velocity", Json::realValue},
																					{"linear damping", Json::realValue},
																					{"angular damping", Json::realValue},
																					{"allow sleep", Json::booleanValue},
																					{"awake", Json::booleanValue},
																					{"fixed rotation", Json::booleanValue},
																					{"bullet", Json::booleanValue},
																					{"active", Json::booleanValue},
																					{"gravity scale", Json::realValue},
																					{"fixtures", Json::arrayValue}});
							b2BodyDef entityBodyComponentDef;
							entityBodyComponentDef.userData = &m_entities[entityName];
							
							//type
							if(body.isMember("type"))
							{
								const Json::Value type = body["type"];
								parseValue(type, "entities." + entityName + ".body.type", {"static", "kinematic", "dynamic"});
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
								parseObject(position, "entities." + entityName + ".body.position", {{"x", Json::realValue}, {"y", Json::realValue}});
								if(position.isMember("x"))
									entityBodyComponentDef.position.x = position["x"].asFloat()*uniqScale;
								if(position.isMember("y"))
									entityBodyComponentDef.position.y = position["y"].asFloat()*uniqScale;
							}
							
							//angle
							if(body.isMember("angle"))
								entityBodyComponentDef.angle = body["angle"].asFloat();
							
							//linear velocity
							if(body.isMember("linear velocity"))
							{
								const Json::Value linearVelocity = body["linear velocity"];
								parseObject(linearVelocity, "entities." + entityName + ".body.linear velocity", {{"x", Json::realValue}, {"y", Json::realValue}});
								if(linearVelocity.isMember("x"))
									entityBodyComponentDef.linearVelocity.x = linearVelocity["x"].asFloat()*uniqScale;
								if(linearVelocity.isMember("y"))
									entityBodyComponentDef.linearVelocity.y = linearVelocity["y"].asFloat()*uniqScale;
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
							
							b2Body* entityBodyComponent = getContext().world.CreateBody(&entityBodyComponentDef);
							if(not m_entities[entityName].has_component<BodyComponent>())
							{
								std::map<std::string, b2Body*> bodies{{partName, entityBodyComponent}};
								m_entities[entityName].assign<BodyComponent>(bodies);
							}
							else
								m_entities[entityName].component<BodyComponent>()->bodies.emplace(partName, entityBodyComponent);
							
							//fixtures
							if(body.isMember("fixtures"))
							{
								const Json::Value fixtures = body["fixtures"];
								parseArray(fixtures, "entities." + entityName + ".body.fixtures", Json::objectValue);
								for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
								{
									const Json::Value fixture = fixtures[i];
									parseObject(fixture, "entities." + entityName + ".body.fixtures" + std::to_string(i), {{"type", Json::stringValue},
																															{"box", Json::objectValue},
																															{"vertices", Json::arrayValue},
																															{"1", Json::objectValue},
																															{"2", Json::objectValue},
																															{"density", Json::realValue},
																															{"friction", Json::realValue},
																															{"restitution", Json::realValue},
																															{"is sensor", Json::booleanValue},
																															{"is foot sensor", Json::booleanValue}});
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
										parseValue(type, "entities." + entityName + ".body.fixtures" + std::to_string(i) + ".type", {"polygon", "edge", "chain", "circle"});

										if(type == "polygon")
										{
											//box
											if(fixture.isMember("box"))
											{
												const Json::Value box = fixtures[i]["box"];
												parseObject(box, "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".box", {{"w", Json::realValue},
																																			{"h", Json::realValue},
																																			{"center x", Json::realValue},
																																			{"center y", Json::realValue},
																																			{"angle", Json::realValue}});
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
													
												polygonShape.SetAsBox((w*uniqScale)/2, (h*uniqScale)/2, {cx*uniqScale, cy*uniqScale}, angle);
											}
											
											//vertices
											else if(fixture.isMember("vertices"))
											{
												const Json::Value vertices = fixtures[i]["vertices"];
												parseArray(vertices, "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".vertices", Json::objectValue);
												for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
												{
													const Json::Value vertice = vertices[j];
													parseObject(vertice, "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".vertices." + std::to_string(j), {{"x", Json::realValue}, {"y", Json::realValue}});
												
													//x
													if(vertice.isMember("x"))
														polygonShape.m_vertices[j].x = vertice["x"].asFloat()*uniqScale;
													
													//y
													if(vertice.isMember("y"))
														polygonShape.m_vertices[j].y = vertice["y"].asFloat()*uniqScale;
												}
											}
											else
												throw std::runtime_error("\"entities." + entityName + ".body.fixtures." + std::to_string(i) + "\" has no geometry.");
											entityFixtureDef.shape = &polygonShape;
										}
										else if(type == "edge")
										{
											float x1{0}, y1{0}, x2{0}, y2{0};
											//Assert that 1 and 2 exist
											requireValues(fixture, "entities." + entityName + ".body.fixtures." + std::to_string(i), {{"1", Json::objectValue}, {"2", Json::objectValue}});
											
											//1
											parseObject(fixture["1"], "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".1", {{"x", Json::realValue},{"y", Json::realValue}});
											//x
											if(fixture["1"].isMember("x"))
												x1 = fixtures[i]["1"]["x"].asFloat();
											//y
											if(fixture["1"].isMember("y"))
												y1 = fixtures[i]["1"]["y"].asFloat();
											
											//2
											parseObject(fixture["2"], "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".2", {{"x", Json::realValue}, {"y", Json::realValue}});
											//x
											if(fixture["2"].isMember("x"))
												x2 = fixtures[i]["2"]["x"].asFloat();
											//y
											if(fixture["2"].isMember("y"))
												y2 = fixtures[i]["2"]["y"].asFloat();
											
											edgeShape.Set({(x1*uniqScale), (y1*uniqScale)}, {(x2*uniqScale), (y2*uniqScale)});
											entityFixtureDef.shape = &edgeShape;
										}
										else if(type == "chain")
										{
											//vertices
											requireValues(fixture, "entities." + entityName + ".body.fixtures." + std::to_string(i), {{"vertices", Json::arrayValue}});
											const Json::Value vertices = fixtures[i]["vertices"];
											parseArray(vertices, "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".vertices", Json::objectValue);
											//Vertices of the chain shape
											for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
											{
												const Json::Value vertice = vertices[j];
												parseObject(vertice, "entities." + entityName + ".body.fixtures." + std::to_string(i) + ".vertices." + std::to_string(j), {{"x", Json::realValue}, {"y", Json::realValue}});
												verticesArray.push_back(b2Vec2(0, 0));
												//x
												if(vertice.isMember("x"))
													verticesArray[j].x = vertice["x"].asFloat()*uniqScale;
												
												//y
												if(vertice.isMember("y"))
													verticesArray[j].y = vertice["y"].asFloat()*uniqScale;
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
						
						//animations
						if(part.isMember("spritesheet animations"))
						{
							const Json::Value animations = part["spritesheet animations"];
							parseObject(animations, "entities." + entityName + ".parts." + partName + ".spritesheet animations", Json::objectValue);
							AnimationsManager<SpriteSheetAnimation> animationsManager;
							for(std::string& animationName : animations.getMemberNames())
							{
								const Json::Value animation = animations[animationName];
								parseObject(animation, "entities." + entityName + ".spritesheet animations." + animationName, {{"importance", Json::intValue},
																																{"duration", Json::realValue},
																																{"loop", Json::booleanValue},
																																{"data", Json::arrayValue}});
								sf::Sprite& spr = m_entities[entityName].component<SpriteComponent>()->sprites[partName];
								SpriteSheetAnimation entityAnimation(spr);
								unsigned short int importance{0};
								float duration{0.f};
								bool loop{false};
								
								//importance
								if(animation.isMember("importance"))
									importance = animation["importance"].asUInt();
								
								//duration
								if(animation.isMember("duration"))
									duration = animation["duration"].asFloat();
								
								//loop
								if(animation.isMember("loop"))
									loop = animation["loop"].asBool();
								
								//frames (=data, because of template implementation)
								if(animation.isMember("data"))
								{
									const Json::Value frames = animation["data"];
									parseArray(frames, "entities." + entityName + ".spritesheet animations." + animationName +  ".data", Json::objectValue);
									for(Json::ArrayIndex i{0}; i < frames.size(); ++i)
									{
										const Json::Value frame = frames[i];
										parseObject(frame, "entities." + entityName + ".spritesheet animations." + animationName + ".data." + std::to_string(i), {{"x", Json::intValue},
																																									{"y", Json::intValue},
																																									{"w", Json::intValue},
																																									{"h", Json::intValue},
																																									{"relative duration", Json::realValue}});
										int x{0}, y{0}, w{0}, h{0};
										float relativeDuration{0.f};
										
										//x
										if(frame.isMember("x"))
											x = frames[i]["x"].asInt();
										
										//y
										if(frame.isMember("y"))
											y = frames[i]["y"].asInt();
										
										//w
										if(frame.isMember("w"))
											w = frames[i]["w"].asInt();
										
										//h
										if(frame.isMember("h"))
											h = frames[i]["h"].asInt();
										
										//relative duration
										if(frame.isMember("relative duration"))
											relativeDuration = frames[i]["relative duration"].asFloat();
										entityAnimation.addFrame(sf::IntRect(static_cast<float>(x)*scale, static_cast<float>(y)*scale, static_cast<float>(w)*scale, static_cast<float>(h)*scale), relativeDuration);
									}
								}
								animationsManager.addAnimation(animationName, entityAnimation, importance, sf::seconds(duration), loop);
							}
							if(not m_entities[entityName].has_component<AnimationsComponent<SpriteSheetAnimation>>())
							{
								std::map<const std::string, AnimationsManager<SpriteSheetAnimation>> animationsManagers{{partName, animationsManager}};
								m_entities[entityName].assign<AnimationsComponent<SpriteSheetAnimation>>(animationsManagers);
							}
							else
								m_entities[entityName].component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers.emplace(partName, animationsManager);
						}
						
						//play animations
						if(part.isMember("play animations"))
						{
							//Assert that some animations are defined
							requireValues(part, "entities." + entityName + ".parts." + partName, {{"spritesheet animations", Json::objectValue}});
							const Json::Value animationsToPlay = part["play animations"];
							parseArray(animationsToPlay, "entities." + entityName + ".parts." + partName + ".play animations", Json::stringValue);
							for(Json::ArrayIndex i{0}; i < animationsToPlay.size(); ++i)
							{
								//Assert that the animation is defined
								requireValues(part["spritesheet animations"], "entities." + entityName + ".parts." + partName + ".spritesheet animations", {{animationsToPlay[i].asString(), Json::objectValue}});
								m_entities[entityName].component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers[partName].play(animationsToPlay[i].asString());
							}
						}
						
						//activate animations
						if(part.isMember("activate animations"))
						{
							requireValues(part, "entities." + entityName + ".parts." + partName, {{"spritesheet animations", Json::objectValue}});
							const Json::Value animationsToActivate = part["activate animations"];
							parseArray(animationsToActivate, "entities." + entityName + ".parts." + partName + ".activate animations", Json::stringValue);
							for(Json::ArrayIndex i{0}; i < animationsToActivate.size(); ++i)
							{
								requireValues(part["spritesheet animations"], "entities." + entityName + ".parts." + partName + ".spritesheet animations", {{animationsToActivate[i].asString(), Json::objectValue}});
								m_entities[entityName].component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers[partName].activate(animationsToActivate[i].asString());
							}
						}
					}
				}
				
				//categories
				if(entity.isMember("categories"))
				{
					const Json::Value categories = entity["categories"];
					//Assert that every value in categories is one of the given values
					parseArray(categories, "entities." + entityName + ".categories", {"player", "scene"});
					unsigned int categoriesInt{0};
					for(Json::ArrayIndex i{0}; i < categories.size(); ++i)
					{
						//For each cateory in the list, add it to the entity's category.
						if(categories[i] == "player")
							categoriesInt |= Category::Player;
						else if(categories[i] == "scene")
							categoriesInt |= Category::Scene;
					}
					m_entities[entityName].assign<CategoryComponent>(categoriesInt);
				}
				
				//actor ID
				if(entity.isMember("actor ID"))
					m_entities[entityName].assign<ActorIDComponent>(entity["actor ID"].asUInt());
				
				//plan
				if(entity.isMember("plan"))
				{
					if(m_entities[entityName].has_component<SpriteComponent>())
						m_entities[entityName].component<SpriteComponent>()->plan = entity["plan"].asFloat();
					else
						throw std::runtime_error("\"entities." + entityName + ".parts.*.sprites\" required by \"entities." + entityName + ".plan\" is not defined.");
				}
					
				
				//walk
				if(entity.isMember("walk"))
				{
					const Json::Value walk = entity["walk"];
					parseObject(walk, "entities." + entityName + ".walk", {{"speed", Json::realValue}, {"effective movement", Json::stringValue}});
					requireValues(walk, "entities." + entityName + ".walk", {{"speed", Json::realValue}});
					
					//speed
					float speed{walk["speed"].asFloat()};
					
					//effective movement
					Direction effectiveMovementEnum{Direction::None};
					if(walk.isMember("effective movement"))
					{
						const Json::Value effectiveMovement = walk["effective movement"];
						parseValue(effectiveMovement, "entities." + entityName + ".walk.effective movement", {"left", "right", "top", "bottom", "none"});
						if(effectiveMovement.asString() == "left")
							effectiveMovementEnum = Direction::Left;
						else if(effectiveMovement.asString() == "right")
							effectiveMovementEnum = Direction::Right;
						else if(effectiveMovement.asString() == "top")
							effectiveMovementEnum = Direction::Top;
						else if(effectiveMovement.asString() == "bottom")
							effectiveMovementEnum = Direction::Bottom;
						else if(effectiveMovement.asString() == "none")
							effectiveMovementEnum = Direction::None;
					}
					m_entities[entityName].assign<WalkComponent>(speed, effectiveMovementEnum);
				}
				
				//jump
				if(entity.isMember("jump"))
					m_entities[entityName].assign<JumpComponent>(entity["jump"].asFloat());
				
				//bend
				if(entity.isMember("bend"))
				{
					const Json::Value bend = entity["bend"];
					parseObject(bend, "entities." + entityName + ".bend", {{"maximum power", Json::realValue}, {"power", Json::realValue}, {"angle", Json::realValue}});
					requireValues(bend, "entities." + entityName + ".bend", {{"maximum power", Json::realValue}});
					
					//maximum power
					float maxPower{bend["maximum power"].asFloat()};
					
					//power
					float power{0.f};
					if(bend.isMember("power"))
						power = bend["power"].asFloat();
					
					//angle
					float angle{0.f};
					if(bend.isMember("angle"))
						angle = bend["angle"].asFloat();
						
					m_entities[entityName].assign<BendComponent>(maxPower, power, angle);
				}
				
				//health
				if(entity.isMember("health"))
				{
					const Json::Value healthObj = entity["health"];
					parseObject(healthObj, "entities." + entityName + ".health", {{"maximum health", Json::realValue}, {"current health", Json::realValue}});
					requireValues(healthObj, "entities." + entityName + ".health", {{"maximum health", Json::realValue}});
					
					//maximum health
					float maxHealth{healthObj["maximum health"].asFloat()};
					
					//health
					float health{maxHealth};
					if(healthObj.isMember("current health"))
						health = healthObj["current health"].asFloat();
						
					m_entities[entityName].assign<HealthComponent>(maxHealth, health);
				}
				
				//stamina
				if(entity.isMember("stamina"))
				{
					const Json::Value staminaObj = entity["stamina"];
					parseObject(staminaObj, "entities." + entityName + ".stamina", {{"maximum stamina", Json::realValue}, {"current stamina", Json::realValue}});
					requireValues(staminaObj, "entities." + entityName + ".stamina", {{"maximum stamina", Json::realValue}});
					
					//maximum stamina
					float maxStamina{staminaObj["maximum stamina"].asFloat()};
					
					//stamina
					float stamina{maxStamina};
					if(staminaObj.isMember("current stamina"))
						stamina = staminaObj["current stamina"].asFloat();
						
					m_entities[entityName].assign<StaminaComponent>(maxStamina, stamina);
				}
				
				//direction
				if(entity.isMember("direction"))
				{
					const Json::Value directionObj = entity["direction"];
					parseObject(directionObj, "entities." + entityName + ".direction", {{"direction", Json::stringValue}, {"move to left", Json::booleanValue}, {"move to right", Json::booleanValue}});
					requireValues(directionObj, "entities." + entityName + ".direction", {{"direction", Json::stringValue}});
					
					//direction
					const Json::Value direction = directionObj["direction"];
					parseValue(direction, "entities." + entityName + ".direction", {"left", "right", "top", "bottom", "none"});
					Direction directionEnum{Direction::None};
					if(direction.asString() == "left")
						directionEnum = Direction::Left;
					else if(direction.asString() == "right")
						directionEnum = Direction::Right;
					else if(direction.asString() == "top")
						directionEnum = Direction::Top;
					else if(direction.asString() == "bottom")
						directionEnum = Direction::Bottom;
					else if(direction.asString() == "none")
						directionEnum = Direction::None;
					
					//move to left
					bool moveToLeft{false};
					if(directionObj.isMember("move to left"))
						moveToLeft = directionObj["move to left"].asBool();
					
					//move to right
					bool moveToRight{false};
					if(directionObj.isMember("move to right"))
						moveToRight = directionObj["move to right"].asBool();
					
					m_entities[entityName].assign<DirectionComponent>(directionEnum, moveToLeft, moveToRight);
				}
				
				//fall
				if(entity.isMember("fall"))
				{
					const Json::Value fall = entity["fall"];
					parseObject(fall, "entities." + entityName + ".fall", {{"in air", Json::booleanValue}, {"contact count", Json::intValue}});
					requireValues(fall, "entities." + entityName + ".fall", {{"in air", Json::booleanValue}, {"contact count", Json::intValue}});
					m_entities[entityName].assign<FallComponent>(fall["in air"].asBool(), fall["contact count"].asUInt());
				}
			}
		}
		
		//joints
		if(root.isMember("joints"))
		{
			const Json::Value joints = root["joints"];
			parseArray(joints, "joints", Json::objectValue);
			for(Json::ArrayIndex i{0}; i < joints.size(); ++i)
			{
				const Json::Value joint = joints[i];
				parseObject(joint, "joints." + std::to_string(i), {{"entity A", Json::stringValue},
																	{"entity B", Json::stringValue},
																	{"part A", Json::stringValue},
																	{"part B", Json::stringValue},
																	{"type", Json::stringValue},
																	{"local anchor A", Json::objectValue},
																	{"local anchor B", Json::objectValue},
																	{"lower angle", Json::realValue},
																	{"upper angle", Json::realValue},
																	{"enable limit", Json::booleanValue},
																	{"max motor torque", Json::realValue},
																	{"enable motor", Json::booleanValue}});
				requireValues(joint, "joints." + std::to_string(i), {{"entity A", Json::stringValue},
																	{"entity B", Json::stringValue},
																	{"part A", Json::stringValue},
																	{"part B", Json::stringValue},
																	{"type", Json::stringValue}});
				std::string entityA = joint["entity A"].asString();
				std::string entityB = joint["entity B"].asString();
				std::string partA = joint["part A"].asString();
				std::string partB = joint["part B"].asString();
				//Assert that somes entities exists
				requireValues(root, "root", {{"entities", Json::objectValue}});
				//Assert that the given entities exist
				requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
				//Assert that that entities have some parts
				requireValues(root["entities"][entityA], "entities." + entityA, {{"parts", Json::objectValue}});
				requireValues(root["entities"][entityB], "entities." + entityB, {{"parts", Json::objectValue}});
				//Assert that that entities have the givens parts
				requireValues(root["entities"][entityA]["parts"], "entities." + entityA + ".parts", {{partA, Json::objectValue}});
				requireValues(root["entities"][entityB]["parts"], "entities." + entityB + ".parts", {{partB, Json::objectValue}});
				//Assert that the given parts have a body
				requireValues(root["entities"][entityA]["parts"][partA], "entities." + entityA + ".parts." + partA, {{"body", Json::objectValue}});
				requireValues(root["entities"][entityB]["parts"][partB], "entities." + entityB + ".parts." + partB, {{"body", Json::objectValue}});
				b2Body * bodyA{m_entities[entityA].component<BodyComponent>()->bodies[partA]};
				b2Body * bodyB{m_entities[entityB].component<BodyComponent>()->bodies[partB]};
							
				//type
				const Json::Value type = joint["type"];
				parseValue(type, "joints." + std::to_string(i) + ".type", {"revolute joint",
																			"distance joint",
																			"friction joint",
																			"gear joint",
																			"motor joint",
																			"prismatic joint",
																			"pulley joint",
																			"rope joint",
																			"weld joint",
																			"wheel joint"});											
				if(type == "revolute joint")
				{
					b2RevoluteJointDef jointDef;
					float xf{0}, yf{0}, xs{0}, ys{0};
					jointDef.bodyA = bodyA;
					jointDef.bodyB =  bodyB;
					
					//local anchor A
					if(joint.isMember("local anchor A"))
					{
						const Json::Value firstAnchor = joint["local anchor A"];
						parseObject(firstAnchor,"joints." + std::to_string(i) + ".local anchor A", {{"x", Json::realValue}, {"y", Json::realValue}});
									
						//x
						if(firstAnchor.isMember("x"))
							xf = firstAnchor["x"].asFloat();
						
						//y
						if(firstAnchor.isMember("y"))
							yf = firstAnchor["y"].asFloat();
					}
					jointDef.localAnchorA = {(xf*uniqScale), (yf*uniqScale)};
					
					//local anchor B
					if(joint.isMember("local anchor B"))
					{
						const Json::Value secondAnchor = joint["local anchor B"];
						parseObject(secondAnchor,"joints." + std::to_string(i) + ".local anchor B", {{"x", Json::realValue}, {"y", Json::realValue}});
						
						//x
						if(secondAnchor.isMember("x"))
							xs = secondAnchor["x"].asFloat();
						
						//y
						if(secondAnchor.isMember("y"))
							ys = secondAnchor["y"].asFloat();
					}
					jointDef.localAnchorB = {(xs*uniqScale), (ys*uniqScale)};
					
					//lower angle
					if(joint.isMember("lower angle"))
						jointDef.lowerAngle = (joint["lower angle"].asFloat() * b2_pi) / 180.f;
					
					//upper angle
					if(joint.isMember("upper angle"))
						jointDef.upperAngle = (joint["upper angle"].asFloat() * b2_pi) / 180.f;
					
					//enable limit
					if(joint.isMember("enable limit"))
						jointDef.enableLimit = joint["enable limit"].asBool();
					
					//max motor torque
					if(joint.isMember("max motor torque"))
						jointDef.maxMotorTorque = joint["max motor torque"].asFloat();
					
					//enable motor
					if(joint.isMember("enable motor"))
						jointDef.enableMotor = joint["enable motor"].asBool();
					
					getContext().world.CreateJoint(&jointDef);
				}
				// TODO: implementer tous les autres types de joints.
				else if(type == "distance joint")
				{
				}
				else if(type == "friction joint")
				{
				}
				else if(type == "gear joint")
				{
				}
				else if(type == "motor joint")
				{
				}
				else if(type == "prismatic joint")
				{
				}
				else if(type == "pulley joint")
				{
				}
				else if(type == "rope joint")
				{
				}
				else if(type == "weld joint")
				{
				}
				else if(type == "wheel joint")
				{
				}
			}
		}
		
		//Load the day sky
		const std::string dayIdentifier{"day sky"};
		sf::Vector2f position{(1920.f/2.f) * scale, 1080.f * scale};
		sf::Vector2f origin{(2900.f/2.f) * scale, (2900.f/2.f) * scale};
		if(not texManager.isLoaded(dayIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(float(m_numberOfPlans*100)/float(m_numberOfPlans+1), LangManager::tr("Loading day sky"));
			texManager.load(dayIdentifier, paths[getContext().parameters.scaleIndex] + "day.png");
		}
		//Create an entity
		m_levelEntities.emplace(dayIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite daySpr(texManager.get(dayIdentifier));
		//Assign origin of the sprite to the center of the day image
		daySpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		std::map<std::string, sf::Sprite> daySprites{{"main", daySpr}};
		std::map<std::string, sf::Vector2f> dayWorldPositions{{"main", {position.x, position.y}}};
		m_levelEntities[dayIdentifier].assign<SpriteComponent>(daySprites, dayWorldPositions, std::numeric_limits<double>::infinity());
		m_levelEntities[dayIdentifier].assign<SkyComponent>(true);
		m_levelEntities[dayIdentifier].assign<CategoryComponent>(Category::Scene);
	
		//Load the night sky
		const std::string nightIdentifier{"night sky"};
		if(not texManager.isLoaded(nightIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(float(m_numberOfPlans*100 + 50)/float(m_numberOfPlans+1), LangManager::tr("Loading night sky"));
			texManager.load(nightIdentifier, paths[getContext().parameters.scaleIndex] + "night.png");
		}
		//Create an entity
		m_levelEntities.emplace(nightIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite nightSpr(texManager.get(nightIdentifier));
		//Assign origin of the sprite to the center of the night image
		nightSpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		std::map<std::string, sf::Sprite> nightSprites{{"main", nightSpr}};
		std::map<std::string, sf::Vector2f> nightWorldPositions{{"main", {position.x, position.y}}};
		m_levelEntities[nightIdentifier].assign<SpriteComponent>(nightSprites, nightWorldPositions, std::numeric_limits<double>::infinity());
		m_levelEntities[nightIdentifier].assign<SkyComponent>(false);
		m_levelEntities[nightIdentifier].assign<CategoryComponent>(Category::Scene);
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Failed to load save file \"" << file << "\"." << std::endl;
		//Clear game content in order to prevent segmentation faults.
		for(auto& entity : m_entities)
		{
			if(entity.second.has_component<BodyComponent>())
			{
				for(auto& partBody : entity.second.component<BodyComponent>()->bodies)
					getContext().world.DestroyBody(partBody.second);
			}
			entity.second.destroy();
		}
		m_entities.clear();
		getContext().world.ClearForces();
	}
	getContext().player.handleInitialInputState(getContext().commandQueue);
	getContext().world.SetContactListener(&m_contactListener);
	getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
	requestStackPop();
	requestStackPush(States::HUD);
	CategoryComponent::Handle categoryComponent;
	//Set the new health and stamina to the HUD
	saveWorld("resources/levels/test.json");
	for(auto entity : getContext().entityManager.entities_with_components(categoryComponent))
	{
		if(categoryComponent->category & Category::Player and entity.has_component<HealthComponent>())
			getContext().eventManager.emit<PlayerHealthChange>(entity.component<HealthComponent>()->health);
		if(categoryComponent->category & Category::Player and entity.has_component<StaminaComponent>())
			getContext().eventManager.emit<PlayerStaminaChange>(entity.component<StaminaComponent>()->stamina);
	}
}
