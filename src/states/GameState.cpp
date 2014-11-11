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

#include <TheLostGirl/states/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_contactListener(),
	m_timeSpeed{1.f},
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 0, 1080},
	m_threadLoad(&GameState::initWorld, this)
{
	m_threadLoad.launch();
}

GameState::~GameState()
{
	for(auto& pair : m_entities)
	{
		if(pair.second.has_component<BodyComponent>())
			getContext().world.DestroyBody(pair.second.component<BodyComponent>()->body);
		pair.second.destroy();
	}
	getContext().world.ClearForces();
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
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
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

void GameState::initWorld()
{
	const std::string filePath{"ressources/levels/save.json"};
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
		std::ifstream saveFileStream(filePath, std::ifstream::binary);
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
				std::string file{m_levelIdentifier + "_" + std::to_string(i)};
				//Path of the image to load
				std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + file + ".png"};
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
						//Identifier of the entity and the texture, e.g. first image loaded in the plan 3 : "3_0"
						std::string textureIdentifier{std::to_string(i) + "_" + std::to_string(j)};
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
							requireValues(replace, "level." + std::to_string(i) + "." + std::to_string(j) + ".replace" + std::to_string(k), {{"x", Json::realValue},
																															{"y", Json::realValue}});
							//Position where the frame should be replaced
							float rx{replace["x"].asFloat()*getContext().parameters.scale};
							float ry{replace["y"].asFloat()*getContext().parameters.scale};
							//Create an entity
							m_entities.emplace(textureIdentifier, getContext().entityManager.create());
							//Create a sprite with the loaded texture
							//Assign the sprite to the entity
							sf::Sprite replaceSpr(texManager.get(textureIdentifier));
							replaceSpr.setPosition(rx, ry);
							m_entities[textureIdentifier].assign<SpriteComponent>(replaceSpr, sf::Vector3f(rx, ry, static_cast<float>(i)));
							m_entities[textureIdentifier].assign<CategoryComponent>(Category::Scene);
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
						//Name of the texture
						std::string textureIdentifier{file + "_" + std::to_string(j)};
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
						m_entities.emplace(textureIdentifier, getContext().entityManager.create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite chunkSpr(texManager.get(textureIdentifier));
						chunkSpr.setPosition(j*chunkSize, 0);
						m_entities[textureIdentifier].assign<SpriteComponent>(chunkSpr, sf::Vector3f(j*chunkSize, 0, static_cast<float>(i)));
						m_entities[textureIdentifier].assign<CategoryComponent>(Category::Scene);
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
				parseObject(entity, "entities." + entityName, {{"sprite", Json::objectValue},
																{"categories", Json::arrayValue},
																{"actor ID", Json::intValue},
																{"walk", Json::objectValue},
																{"jump", Json::realValue},
																{"bend", Json::objectValue},
																{"direction", Json::objectValue},
																{"fall", Json::objectValue},
																{"body", Json::objectValue},
																{"spritesheet animations", Json::objectValue},
																{"play animations", Json::arrayValue},
																{"activate animations", Json::arrayValue}});
				m_entities.emplace(entityName, getContext().entityManager.create());
				
				//sprite
				if(entity.isMember("sprite"))
				{
					const Json::Value sprite = entity["sprite"];
					parseObject(sprite, "entities." + entityName + ".sprite", {{"plan", Json::realValue}, {"identifier", Json::stringValue}});
					//Assert that the identifier is defined
					requireValues(sprite, "entities." + entityName + ".sprite", {{"identifier", Json::stringValue}});
					//Assert that the identifier is one of the given values
					parseValue(sprite["identifier"], "entities." + entityName + ".sprite.identifier", {"archer", "bow", "arms"});
					//Default value for the plan
					float plan = m_referencePlan;
					
					//plan
					if(sprite.isMember("plan"))
						plan = sprite["plan"].asFloat();
					
					//Assign the sprite to the entity
					m_entities[entityName].assign<SpriteComponent>(sf::Sprite(getContext().textureManager.get(sprite["identifier"].asString())), sf::Vector3f(0, 0, plan));
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
				
				//body
				if(entity.isMember("body"))
				{
					const Json::Value body = entity["body"];
					parseObject(body, "entities." + entityName + ".body", {{"type", Json::stringValue},
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
					m_entities[entityName].assign<BodyComponent>(entityBodyComponent);
					
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
																													{"is sensor", Json::booleanValue}});
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
								entityFixtureDef.density = fixtures[i]["density"].asFloat();
							
							//friction
							if(fixture.isMember("friction"))
								entityFixtureDef.friction = fixtures[i]["friction"].asFloat();
							
							//restitution
							if(fixture.isMember("restitution"))
								entityFixtureDef.restitution = fixtures[i]["restitution"].asFloat();
							
							//is sensor
							if(fixture.isMember("is sensor"))
								entityFixtureDef.isSensor = fixtures[i]["is sensor"].asBool();
							entityBodyComponent->CreateFixture(&entityFixtureDef);
						}
					}
				}
				
				//animations
				if(entity.isMember("spritesheet animations"))
				{
					const Json::Value animations = entity["spritesheet animations"];
					parseObject(animations, "entities." + entityName + ".spritesheet animations", Json::objectValue);
					AnimationsComponent::Handle animationsComponent = m_entities[entityName].assign<AnimationsComponent>(Animations());
					for(std::string& animationName : animations.getMemberNames())
					{
						const Json::Value animation = animations[animationName];
						parseObject(animation, "entities." + entityName + ".spritesheet animations." + animationName, {{"importance", Json::intValue},
																														{"duration", Json::realValue},
																														{"loop", Json::booleanValue},
																														{"frames", Json::arrayValue}});
						SpriteSheetAnimation entityAnimation;
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
						
						//frames
						if(animation.isMember("frames"))
						{
							const Json::Value frames = animation["frames"];
							parseArray(frames, "entities." + entityName + ".spritesheet animations." + animationName +  ".frames", Json::objectValue);
							for(Json::ArrayIndex i{0}; i < frames.size(); ++i)
							{
								const Json::Value frame = frames[i];
								parseObject(frame, "entities." + entityName + ".spritesheet animations." + animationName + ".frames." + std::to_string(i), {{"x", Json::intValue},
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
						animationsComponent->animations.addAnimation(animationName, entityAnimation, importance, sf::seconds(duration), loop);
					}
					
					//play animations
					if(entity.isMember("play animations"))
					{
						//Assert that some animations are defined
						requireValues(entity, "entities." + entityName, {{"spritesheet animations", Json::objectValue}});
						const Json::Value animationsToPlay = entity["play animations"];
						parseArray(animationsToPlay, "entities." + entityName + ".play animations", Json::stringValue);
						for(Json::ArrayIndex i{0}; i < animationsToPlay.size(); ++i)
						{
							//Assert that the animation is defined
							requireValues(entity["spritesheet animations"], "entities." + entityName + ".spritesheet animations", {{animationsToPlay[i].asString(), Json::objectValue}});
							m_entities[entityName].component<AnimationsComponent>()->animations.play(animationsToPlay[i].asString());
						}
					}
					
					//activate animations
					if(entity.isMember("activate animations"))
					{
						requireValues(entity, "entities." + entityName, {{"spritesheet animations", Json::objectValue}});
						const Json::Value animationsToActivate = entity["activate animations"];
						parseArray(animationsToActivate, "entities." + entityName + ".activate animations", Json::stringValue);
						for(Json::ArrayIndex i{0}; i < animationsToActivate.size(); ++i)
						{
							requireValues(entity["spritesheet animations"], "entities." + entityName + ".spritesheet animations", {{animationsToActivate[i].asString(), Json::objectValue}});
							m_entities[entityName].component<AnimationsComponent>()->animations.activate(animationsToActivate[i].asString());
						}
					}
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
				parseObject(joint, "joints." + std::to_string(i), {{"first", Json::stringValue},
																	{"second", Json::stringValue},
																	{"type", Json::stringValue},
																	{"local anchor first", Json::objectValue},
																	{"local anchor second", Json::objectValue},
																	{"lower angle", Json::realValue},
																	{ "upper angle", Json::realValue},
																	{"enable limit", Json::booleanValue},
																	{"max motor torque", Json::realValue},
																	{"enable motor", Json::booleanValue}});
				requireValues(joint, "joints." + std::to_string(i), {{"first", Json::stringValue},
																	{"second", Json::stringValue},
																	{"type", Json::stringValue}});
				std::string first = joint["first"].asString();
				std::string second = joint["second"].asString();
				//Assert that theses entities are defined
				requireValues(root, "root", {{"entities", Json::objectValue}});
				requireValues(root["entities"], "entities", {{first, Json::objectValue}, {second, Json::objectValue}});
				if(not m_entities[first].has_component<BodyComponent>())
					throw std::runtime_error("\"entities." + first + "\" required by \"joints." + std::to_string(i) + ".first\" has no body member.");
				if(not m_entities[second].has_component<BodyComponent>())
					throw std::runtime_error("\"entities." + second + "\" required by \"joints." + std::to_string(i) + ".second\" has no body member.");
				b2Body * bodyFirst{m_entities[first].component<BodyComponent>()->body};
				b2Body * bodySecond{m_entities[second].component<BodyComponent>()->body};
							
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
					jointDef.bodyA = bodyFirst;
					jointDef.bodyB =  bodySecond;
					
					//local anchor first
					if(joint.isMember("local anchor first"))
					{
						const Json::Value firstAnchor = joint["local anchor first"];
						parseObject(firstAnchor,"joints." + std::to_string(i) + ".local anchor first", {{"x", Json::realValue}, {"y", Json::realValue}});
									
						//x
						if(firstAnchor.isMember("x"))
							xf = firstAnchor["x"].asFloat();
						
						//y
						if(firstAnchor.isMember("y"))
							yf = firstAnchor["y"].asFloat();
					}
					jointDef.localAnchorA = {(xf*uniqScale), (yf*uniqScale)};
					
					//local anchor second
					if(joint.isMember("local anchor second"))
					{
						const Json::Value secondAnchor = joint["local anchor second"];
						parseObject(secondAnchor,"joints." + std::to_string(i) + ".local anchor second", {{"x", Json::realValue}, {"y", Json::realValue}});
						
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
		m_entities.emplace(dayIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite daySpr(texManager.get(dayIdentifier));
		//Assign origin of the sprite to the center of the day image
		daySpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		m_entities[dayIdentifier].assign<SpriteComponent>(daySpr, sf::Vector3f(position.x, position.y, std::numeric_limits<double>::infinity()));
		m_entities[dayIdentifier].assign<SkyComponent>(true);
		m_entities[dayIdentifier].assign<CategoryComponent>(Category::Scene);
	
		//Load the night sky
		const std::string nightIdentifier{"night sky"};
		if(not texManager.isLoaded(nightIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(float(m_numberOfPlans*100 + 50)/float(m_numberOfPlans+1), LangManager::tr("Loading night sky"));
			texManager.load(nightIdentifier, paths[getContext().parameters.scaleIndex] + "night.png");
		}
		//Create an entity
		m_entities.emplace(nightIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite nightSpr(texManager.get(nightIdentifier));
		//Assign origin of the sprite to the center of the night image
		nightSpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		m_entities[nightIdentifier].assign<SpriteComponent>(nightSpr, sf::Vector3f(position.x, position.y, std::numeric_limits<double>::infinity()));
		m_entities[nightIdentifier].assign<SkyComponent>(false);
		m_entities[nightIdentifier].assign<CategoryComponent>(Category::Scene);
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Failed to load save file \"" << filePath << "\"." << std::endl;
		//Clear game content in order to prevent segmentation faults.
		for(auto& pair : m_entities)
		{
			if(pair.second.has_component<BodyComponent>())
				getContext().world.DestroyBody(pair.second.component<BodyComponent>()->body);
			pair.second.destroy();
		}
		m_entities.clear();
		getContext().world.ClearForces();
	}
	getContext().player.handleInitialInputState(getContext().commandQueue);
	getContext().world.SetContactListener(&m_contactListener);
	getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
	requestStackPop();
}