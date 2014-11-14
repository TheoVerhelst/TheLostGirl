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
		const float scale = getContext().parameters.scale;
		const float pixelScale = getContext().parameters.pixelScale;
		const float uniqScale = scale / pixelScale;//The pixel/meters scale at the maximum resolution, about 1.f/120.f
		Json::Value root;//Will contains the root value after serializing.
		Json::StyledWriter writer;
		std::ofstream saveFileStream(file, std::ofstream::binary);
		for(auto& entity : m_entities)
		{
			if(entity.second.has_component<BodyComponent>())
				root["entities"][entity.first]["body"] = serialize(entity.second.component<BodyComponent>(), uniqScale);
			
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
							SpriteComponent::Handle sprComp = m_levelEntities[replaceIdentifier].assign<SpriteComponent>();
							sprComp->sprites = sprites;
							sprComp->worldPositions = worldPositions;
							sprComp->plan = static_cast<float>(i);
							CategoryComponent::Handle catComp = m_levelEntities[replaceIdentifier].assign<CategoryComponent>();
							catComp->category = Category::Scene;
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
						SpriteComponent::Handle sprComp = m_levelEntities[textureIdentifier].assign<SpriteComponent>();
						sprComp->sprites = sprites;
						sprComp->worldPositions = worldPositions;
						sprComp->plan = static_cast<float>(i);
						CategoryComponent::Handle catComp = m_levelEntities[textureIdentifier].assign<CategoryComponent>();
						catComp->category = Category::Scene;
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
																{"sprite", Json::objectValue},
																{"body", Json::objectValue},
																{"spritesheet animations", Json::objectValue},
																{"plan", Json::realValue},
																{"walk", Json::objectValue},
																{"jump", Json::objectValue},
																{"bend", Json::objectValue},
																{"health", Json::objectValue},
																{"stamina", Json::objectValue},
																{"direction", Json::objectValue},
																{"fall", Json::objectValue}});
				m_entities.emplace(entityName, getContext().entityManager.create());
				
				//sprite
				if(entity.isMember("sprite"))
				{
					const Json::Value sprites = entity["sprite"];
					parseObject(sprites, "entities." + entityName + ".sprite", Json::objectValue);
					for(std::string& partName : sprites.getMemberNames())
					{
						const Json::Value sprite = sprites[partName];
						parseObject(sprite, "entities." + entityName + ".sprite." + partName, {{"identifier", Json::stringValue}, {"position", Json::objectValue}});
						requireValues(sprite, "entities." + entityName + ".sprite." + partName, {{"identifier", Json::stringValue}});
						parseValue(sprite["identifier"], "entities." + entityName + ".sprite." + partName + ".identifier", {"archer", "bow", "arms"});
						
						//position
						if(sprite.isMember("position"))
							requireValues(sprite["position"], "entities." + entityName + ".sprite." + partName + ".position", {{"x", Json::realValue}, {"y", Json::realValue}});
					}
					deserialize(sprites, m_entities[entityName].assign<SpriteComponent>(), texManager, scale);
				}
					
				//body
				if(entity.isMember("body"))
				{
					const Json::Value bodies = entity["body"];
					parseObject(bodies, "entities." + entityName + ".body", Json::objectValue);
					for(std::string& partName : bodies.getMemberNames())
					{
						const Json::Value body = bodies[partName];
						parseObject(body, "entities." + entityName + ".body." + partName, {{"type", Json::stringValue},
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
						
						//type
						if(body.isMember("type"))
							parseValue(body["type"], "entities." + entityName + ".body." + partName + ".type", {"static", "kinematic", "dynamic"});
						
						//position
						if(body.isMember("position"))
							requireValues(body["position"], "entities." + entityName + ".body." + partName + ".position", {{"x", Json::realValue}, {"y", Json::realValue}});
						
						//linear velocity
						if(body.isMember("linear velocity"))
							parseObject(body["linear velocity"], "entities." + entityName + ".body." + partName + ".linear velocity", {{"x", Json::realValue}, {"y", Json::realValue}});
						
						//fixtures
						if(body.isMember("fixtures"))
						{
							const Json::Value fixtures = body["fixtures"];
							parseArray(fixtures, "entities." + entityName + ".body." + partName + ".fixtures", Json::objectValue);
							for(Json::ArrayIndex i{0}; i < fixtures.size(); ++i)
							{
								const Json::Value fixture = fixtures[i];
								parseObject(fixture, "entities." + entityName + ".body." + partName + ".fixtures" + std::to_string(i), {{"type", Json::stringValue},
																														{"box", Json::objectValue},
																														{"vertices", Json::arrayValue},
																														{"1", Json::objectValue},
																														{"2", Json::objectValue},
																														{"density", Json::realValue},
																														{"friction", Json::realValue},
																														{"restitution", Json::realValue},
																														{"is sensor", Json::booleanValue},
																														{"is foot sensor", Json::booleanValue}});
								//type
								if(fixture.isMember("type"))
								{
									const Json::Value type = fixtures[i]["type"];
									parseValue(type, "entities." + entityName + ".body." + partName + ".fixtures" + std::to_string(i) + ".type", {"polygon", "edge", "chain", "circle"});

									if(type == "polygon")
									{
										//box
										if(fixture.isMember("box"))
											parseObject(fixtures[i]["box"], "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".box", {{"w", Json::realValue},
																																		{"h", Json::realValue},
																																		{"center x", Json::realValue},
																																		{"center y", Json::realValue},
																																		{"angle", Json::realValue}});
										//vertices
										else if(fixture.isMember("vertices"))
										{
											const Json::Value vertices = fixtures[i]["vertices"];
											parseArray(vertices, "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".vertices", Json::objectValue);
											for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
												requireValues(vertices[j], "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".vertices." + std::to_string(j), {{"x", Json::realValue}, {"y", Json::realValue}});
										}
										else
											throw std::runtime_error("\"entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + "\" has no geometry.");
									}
									else if(type == "edge")
									{
										requireValues(fixture, "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i), {{"1", Json::objectValue}, {"2", Json::objectValue}});
										//1
										requireValues(fixture["1"], "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".1", {{"x", Json::realValue},{"y", Json::realValue}});
										//2
										requireValues(fixture["2"], "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".2", {{"x", Json::realValue}, {"y", Json::realValue}});
									}
									else if(type == "chain")
									{
										//vertices
										requireValues(fixture, "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i), {{"vertices", Json::arrayValue}});
										const Json::Value vertices = fixtures[i]["vertices"];
										parseArray(vertices, "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".vertices", Json::objectValue);
										//Vertices of the chain shape
										for(Json::ArrayIndex j{0}; j < vertices.size(); ++j)
											requireValues(vertices[j], "entities." + entityName + ".body." + partName + ".fixtures." + std::to_string(i) + ".vertices." + std::to_string(j), {{"x", Json::realValue}, {"y", Json::realValue}});
									}
									// TODO: implementer l'autre type de shapes.
									else if(type == "circle")
									{
									}
								}
							}
						}
					}
					deserialize(bodies, m_entities[entityName].assign<BodyComponent>(), getContext().world, uniqScale);
					//Assign user data to every body of the entity
					for(auto& bodyPair : m_entities[entityName].component<BodyComponent>()->bodies)
						bodyPair.second->SetUserData(&m_entities[entityName]);
				}
				//spritesheet animations
				if(entity.isMember("spritesheet animations"))
				{
					const Json::Value animationsManagers = entity["spritesheet animations"];
					parseObject(animationsManagers, "entities." + entityName + ".spritesheet animations", Json::objectValue);
					for(std::string& partName : animationsManagers.getMemberNames())
					{
						const Json::Value animations = animationsManagers[partName];
						parseObject(animations, "entities." + entityName + ".spritesheet animations." + partName, Json::objectValue);
						for(std::string& animationName : animations.getMemberNames())
						{
							const Json::Value animation = animations[animationName];
							parseObject(animation, "entities." + entityName + ".spritesheet animations." + partName + "." + animationName, {{"importance", Json::intValue},
																															{"duration", Json::realValue},
																															{"loop", Json::booleanValue},
																															{"data", Json::arrayValue}});
							//frames (=data, because of template implementation)
							if(animation.isMember("data"))
							{
								const Json::Value frames = animation["data"];
								parseArray(frames, "entities." + entityName + ".spritesheet animations." + partName + "." + animationName +  ".data", Json::objectValue);
								for(Json::ArrayIndex i{0}; i < frames.size(); ++i)
									requireValues(frames[i], "entities." + entityName + ".spritesheet animations." + partName + "." + animationName + ".data." + std::to_string(i), {{"x", Json::intValue},
																																								{"y", Json::intValue},
																																								{"w", Json::intValue},
																																								{"h", Json::intValue},
																																								{"relative duration", Json::realValue}});
							}
						}
					}
					deserialize(animationsManagers, m_entities[entityName].assign<AnimationsComponent<SpriteSheetAnimation>>(),  m_entities[entityName].component<SpriteComponent>(), scale);
				}
				
				//categories
				if(entity.isMember("categories"))
				{
					//Assert that every value in categories is one of the given values
					parseArray(entity["categories"], "entities." + entityName + ".categories", {"player", "scene"});
					deserialize(entity["categories"], m_entities[entityName].assign<CategoryComponent>());
				}
				
				//actor ID
				if(entity.isMember("actor ID"))
					deserialize(entity["actor ID"], m_entities[entityName].assign<ActorIDComponent>());
				
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
					parseObject(entity["walk"], "entities." + entityName + ".walk", {{"speed", Json::realValue}, {"effective movement", Json::stringValue}});
					requireValues(entity["walk"], "entities." + entityName + ".walk", {{"speed", Json::realValue}});
					deserialize(entity["walk"], m_entities[entityName].assign<WalkComponent>());
				}
				
				//jump
				if(entity.isMember("jump"))
				{
					parseObject(entity["jump"], "entities." + entityName + ".jump", {{"strength", Json::realValue}});
					requireValues(entity["jump"], "entities." + entityName + ".jump", {{"strength", Json::realValue}});
					deserialize(entity["jump"], m_entities[entityName].assign<JumpComponent>());
				}
				
				//bend
				if(entity.isMember("bend"))
				{
					parseObject(entity["bend"], "entities." + entityName + ".bend", {{"maximum power", Json::realValue}, {"power", Json::realValue}, {"angle", Json::realValue}});
					requireValues(entity["bend"], "entities." + entityName + ".bend", {{"maximum power", Json::realValue}});
					deserialize(entity["bend"], m_entities[entityName].assign<BendComponent>());
				}
				
				//health
				if(entity.isMember("health"))
				{
					parseObject(entity["health"], "entities." + entityName + ".health", {{"maximum health", Json::realValue}, {"current health", Json::realValue}});
					requireValues(entity["health"], "entities." + entityName + ".health", {{"maximum health", Json::realValue}});
					deserialize(entity["health"], m_entities[entityName].assign<HealthComponent>());
				}
				
				//stamina
				if(entity.isMember("stamina"))
				{
					const Json::Value staminaObj = entity["stamina"];
					parseObject(staminaObj, "entities." + entityName + ".stamina", {{"maximum stamina", Json::realValue}, {"current stamina", Json::realValue}});
					requireValues(staminaObj, "entities." + entityName + ".stamina", {{"maximum stamina", Json::realValue}});
					deserialize(entity["stamina"], m_entities[entityName].assign<StaminaComponent>());
				}
				
				//direction
				if(entity.isMember("direction"))
				{
					parseObject(entity["direction"], "entities." + entityName + ".direction", {{"direction", Json::stringValue}, {"move to left", Json::booleanValue}, {"move to right", Json::booleanValue}});
					requireValues(entity["direction"], "entities." + entityName + ".direction", {{"direction", Json::stringValue}});
					parseValue(entity["direction"]["direction"], "entities." + entityName + ".direction", {"left", "right", "top", "bottom", "none"});
					deserialize(entity["direction"], m_entities[entityName].assign<DirectionComponent>());
				}
				
				//fall
				if(entity.isMember("fall"))
				{
					parseObject(entity["fall"], "entities." + entityName + ".fall", {{"in air", Json::booleanValue}, {"contact count", Json::intValue}});
					requireValues(entity["fall"], "entities." + entityName + ".fall", {{"in air", Json::booleanValue}, {"contact count", Json::intValue}});
					deserialize(entity["fall"], m_entities[entityName].assign<FallComponent>());
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
																	
				Joint jointData;
				jointData.entityA = joint["entity A"].asString();
				jointData.entityB = joint["entity B"].asString();
				jointData.partA = joint["part A"].asString();
				jointData.partB = joint["part B"].asString();
				//Assert that somes entities exists
				requireValues(root, "root", {{"entities", Json::objectValue}});
				//Assert that the given entities exist
				requireValues(root["entities"], "entities", {{jointData.entityA, Json::objectValue}, {jointData.entityB, Json::objectValue}});
				//Assert that that entities have some bodies
				requireValues(root["entities"][jointData.entityA], "entities." + jointData.entityA, {{"body", Json::objectValue}});
				requireValues(root["entities"][jointData.entityB], "entities." + jointData.entityB, {{"body", Json::objectValue}});
				//Assert that that entities have the givens parts
				requireValues(root["entities"][jointData.entityA]["body"], "entities." + jointData.entityA + ".body", {{jointData.partA, Json::objectValue}});
				requireValues(root["entities"][jointData.entityB]["body"], "entities." + jointData.entityB + ".body", {{jointData.partB, Json::objectValue}});
				b2Body * bodyA{m_entities[jointData.entityA].component<BodyComponent>()->bodies[jointData.partA]};
				b2Body * bodyB{m_entities[jointData.entityB].component<BodyComponent>()->bodies[jointData.partB]};
							
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
					jointData.type = e_revoluteJoint;
					jointData.definition = new b2RevoluteJointDef();
					b2RevoluteJointDef* castedJointDef = static_cast<b2RevoluteJointDef*>(jointData.definition);
					float xA{0}, yA{0}, xB{0}, yB{0};
					castedJointDef->bodyA = bodyA;
					castedJointDef->bodyB = bodyB;
					
					//local anchor A
					if(joint.isMember("local anchor A"))
					{
						const Json::Value firstAnchor = joint["local anchor A"];
						requireValues(firstAnchor,"joints." + std::to_string(i) + ".local anchor A", {{"x", Json::realValue}, {"y", Json::realValue}});
									
						//x
						xA = firstAnchor["x"].asFloat();
						
						//y
						yA = firstAnchor["y"].asFloat();
					}
					castedJointDef->localAnchorA = {(xA*uniqScale), (yA*uniqScale)};
					
					//local anchor B
					if(joint.isMember("local anchor B"))
					{
						const Json::Value secondAnchor = joint["local anchor B"];
						parseObject(secondAnchor,"joints." + std::to_string(i) + ".local anchor B", {{"x", Json::realValue}, {"y", Json::realValue}});
						
						//x
						if(secondAnchor.isMember("x"))
							xB = secondAnchor["x"].asFloat();
						
						//y
						if(secondAnchor.isMember("y"))
							yB = secondAnchor["y"].asFloat();
					}
					castedJointDef->localAnchorB = {(xB*uniqScale), (yB*uniqScale)};
					
					//lower angle
					if(joint.isMember("lower angle"))
						castedJointDef->lowerAngle = (joint["lower angle"].asFloat() * b2_pi) / 180.f;
					
					//upper angle
					if(joint.isMember("upper angle"))
						castedJointDef->upperAngle = (joint["upper angle"].asFloat() * b2_pi) / 180.f;
					
					//enable limit
					if(joint.isMember("enable limit"))
						castedJointDef->enableLimit = joint["enable limit"].asBool();
					
					//max motor torque
					if(joint.isMember("max motor torque"))
						castedJointDef->maxMotorTorque = joint["max motor torque"].asFloat();
					
					//enable motor
					if(joint.isMember("enable motor"))
						castedJointDef->enableMotor = joint["enable motor"].asBool();
					
					getContext().world.CreateJoint(castedJointDef);
					m_joints.push_back(jointData);
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
		SpriteComponent::Handle sprComp = m_levelEntities[dayIdentifier].assign<SpriteComponent>();
		sprComp->sprites = daySprites;
		sprComp->worldPositions = dayWorldPositions;
		sprComp->plan = std::numeric_limits<float>::infinity();
		CategoryComponent::Handle catComp = m_levelEntities[dayIdentifier].assign<CategoryComponent>();
		catComp->category = Category::Scene;
		SkyComponent::Handle skyComp = m_levelEntities[dayIdentifier].assign<SkyComponent>();
		skyComp->day = true;
	
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
		sprComp = m_levelEntities[nightIdentifier].assign<SpriteComponent>();
		sprComp->sprites = nightSprites;
		sprComp->worldPositions = nightWorldPositions;
		sprComp->plan = std::numeric_limits<float>::infinity();
		catComp = m_levelEntities[nightIdentifier].assign<CategoryComponent>();
		catComp->category = Category::Scene;
		skyComp = m_levelEntities[nightIdentifier].assign<SkyComponent>();
		skyComp->day = false;
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
//		if(categoryComponent->category & Category::Player and entity.has_component<HealthComponent>())
//			getContext().eventManager.emit<PlayerHealthChange>(entity.component<HealthComponent>()->health);
//		if(categoryComponent->category & Category::Player and entity.has_component<StaminaComponent>())
//			getContext().eventManager.emit<PlayerStaminaChange>(entity.component<StaminaComponent>()->stamina);
	}
}
