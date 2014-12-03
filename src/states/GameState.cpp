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
#include <TheLostGirl/SkyAnimation.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/JointRoles.h>
#include <TheLostGirl/serialization.h>
#include <TheLostGirl/PendingChanges.h>

#include <TheLostGirl/states/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_sceneEntities(),
	m_sceneEntitiesData(),
	m_contactListener(getContext()),
	m_contactFilter(),
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
//	saveWorld("resources/levels/save.json");
	for(auto& entity : m_entities)
	{
		if(entity.second.has_component<BodyComponent>())
		{
			for(auto& partBody : entity.second.component<BodyComponent>()->bodies)
				getContext().world.DestroyBody(partBody.second);
		}
		entity.second.destroy();
	}
	for(auto& sceneEntity : m_sceneEntities)
		sceneEntity.second.destroy();
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
	getContext().systemManager.update<PendingChangesSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ScrollingSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if((event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		or event.type == sf::Event::LostFocus)
		requestStackPush(States::Pause);
	
	getContext().player.handleEvent(event, getContext().pendingChanges.commandQueue);
	//Update the drag and drop state
	bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);
	return false;
}

void GameState::saveWorld(const std::string& file)
{
		const float pixelByMeter{getContext().parameters.pixelByMeter};//The pixel/meters scale at the maximum resolution, about 1.f/120.f
		Json::Value root;//Will contains the root value after serializing.
		Json::StyledWriter writer;
		std::ofstream saveFileStream(file, std::ofstream::binary);
		for(auto& entity : m_entities)
		{
			if(entity.second.has_component<BodyComponent>())
				root["entities"][entity.first]["body"] = serialize(entity.second.component<BodyComponent>(), pixelByMeter);
			
			if(entity.second.has_component<SpriteComponent>())
				root["entities"][entity.first]["sprites"] = serialize(entity.second.component<SpriteComponent>(), getContext().textureManager);
			
			if(entity.second.has_component<TransformComponent>())
				root["entities"][entity.first]["transforms"] = serialize(entity.second.component<TransformComponent>());
			
			if(entity.second.has_component<InventoryComponent>())
				root["entities"][entity.first]["inventory"] = serialize(entity.second.component<InventoryComponent>(), m_entities);
			
			if(entity.second.has_component<QuiverComponent>())
				root["entities"][entity.first]["quiver"] = serialize(entity.second.component<QuiverComponent>(), m_entities);
			
			if(entity.second.has_component<AnimationsComponent<SpriteSheetAnimation>>())
				root["entities"][entity.first]["spritesheet animations"] = serialize(entity.second.component<AnimationsComponent<SpriteSheetAnimation>>());
			
			if(entity.second.has_component<DirectionComponent>())
				root["entities"][entity.first]["direction"] = serialize(entity.second.component<DirectionComponent>());
			
			if(entity.second.has_component<CategoryComponent>())
				root["entities"][entity.first]["categories"] = serialize(entity.second.component<CategoryComponent>());
			
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
			
			if(entity.second.has_component<WindFrictionComponent>())
				root["entities"][entity.first]["wind friction"] = serialize(entity.second.component<WindFrictionComponent>());
		}
		
		//time
		root["time"]["speed"] = m_timeSpeed;
		root["time"]["date"] = getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds();
		
		//level data
		root["level"]["identifier"] = m_levelIdentifier;
		root["level"]["box"]["x"] = m_levelRect.left;
		root["level"]["box"]["y"] = m_levelRect.top;
		root["level"]["box"]["w"] = m_levelRect.width;
		root["level"]["box"]["h"] = m_levelRect.height;
		root["level"]["number of plans"] = m_numberOfPlans;
		root["level"]["reference plan"] = m_referencePlan;
		
		//replaces
		for(auto& planData : m_sceneEntitiesData)
		{
			for(size_t i{0}; i < planData.second.size(); ++i)
			{
				root["level"]["replaces"][planData.first][i]["origin"]["x"] = planData.second[i].origin.left;
				root["level"]["replaces"][planData.first][i]["origin"]["y"] = planData.second[i].origin.top;
				root["level"]["replaces"][planData.first][i]["origin"]["w"] = planData.second[i].origin.width;
				root["level"]["replaces"][planData.first][i]["origin"]["h"] = planData.second[i].origin.height;
				for(Json::ArrayIndex j{0}; j < planData.second[i].replaces.size(); ++j)
				{
					root["level"]["replaces"][planData.first][i]["replaces"][j]["x"] = planData.second[i].replaces[j].x;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["y"] = planData.second[i].replaces[j].y;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["z"] = planData.second[i].replaces[j].z;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["angle"] = planData.second[i].replaces[j].angle;
				}
			}
		}
		
		//joints
		for(b2Joint* joint{getContext().world.GetJointList()}; joint; joint = joint->GetNext())
		{
			b2Body* bodyA{joint->GetBodyA()};//Get the body A
			entityx::Entity entityA{*static_cast<entityx::Entity*>(bodyA->GetUserData())};//Get the entity A from the user data
			assert(isMember(m_entities, entityA));
			std::string entityAName{getKey(m_entities, entityA)};//Get the name of the entity A
			assert(isMember(entityA.component<BodyComponent>()->bodies, bodyA));//Assert that the body A is in the body component of the entity A
			std::string partAName{getKey(entityA.component<BodyComponent>()->bodies, bodyA)};//Get the name of the part A
			
			b2Body* bodyB{joint->GetBodyB()};//Get the body B
			entityx::Entity entityB{*static_cast<entityx::Entity*>(bodyB->GetUserData())};//Get the entity B from the user data
			assert(isMember(m_entities, entityB));
			std::string entityBName{getKey(m_entities, entityB)};//Get the name of the entity B
			assert(isMember(entityB.component<BodyComponent>()->bodies, bodyB));//Assert that the body B is in the body component of entity B
			std::string partBName{getKey(entityB.component<BodyComponent>()->bodies, bodyB)};//Get the name of the part B
			switch(joint->GetType())
			{
				case e_revoluteJoint:
				{
					b2RevoluteJoint* castedJoint{static_cast<b2RevoluteJoint*>(joint)};
					root["joints"]["revolute joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["revolute joints"].size()-1};
					root["joints"]["revolute joints"][last]["entity A"] = entityAName;
					root["joints"]["revolute joints"][last]["entity B"] = entityBName;
					root["joints"]["revolute joints"][last]["part A"] = partAName;
					root["joints"]["revolute joints"][last]["part B"] = partBName;
					root["joints"]["revolute joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["revolute joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["revolute joints"][last]["lower angle"] = (castedJoint->GetLowerLimit() / b2_pi) * 180.f;
					root["joints"]["revolute joints"][last]["upper angle"] = (castedJoint->GetUpperLimit() / b2_pi) * 180.f;
					root["joints"]["revolute joints"][last]["enable limit"] = castedJoint->IsLimitEnabled();
					root["joints"]["revolute joints"][last]["max motor torque"] = castedJoint->GetMaxMotorTorque();
					root["joints"]["revolute joints"][last]["enable motor"] = castedJoint->IsMotorEnabled();
					if(jointHasRole(joint, JointRole::BendingAngle))
						root["joints"]["revolute joints"][last]["roles"].append("bending angle");
					break;
				}
				
				case e_prismaticJoint:
				{
					b2PrismaticJoint* castedJoint{static_cast<b2PrismaticJoint*>(joint)};
					root["joints"]["prismatic joints"].append(Json::objectValue);
					Json::ArrayIndex last{root["joints"]["prismatic joints"].size()-1};
					root["joints"]["prismatic joints"][last]["entity A"] = entityAName;
					root["joints"]["prismatic joints"][last]["entity B"] = entityBName;
					root["joints"]["prismatic joints"][last]["part A"] = partAName;
					root["joints"]["prismatic joints"][last]["part B"] = partBName;
					root["joints"]["prismatic joints"][last]["local anchor A"]["x"] = castedJoint->GetLocalAnchorA().x*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor A"]["y"] = castedJoint->GetLocalAnchorA().y*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor B"]["x"] = castedJoint->GetLocalAnchorB().x*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local anchor B"]["y"] = castedJoint->GetLocalAnchorB().y*pixelByMeter;
					root["joints"]["prismatic joints"][last]["local axis A"]["x"] = castedJoint->GetLocalAxisA().x;
					root["joints"]["prismatic joints"][last]["local axis A"]["y"] = castedJoint->GetLocalAxisA().y;
					root["joints"]["prismatic joints"][last]["enable limit"] = castedJoint->IsLimitEnabled();
					root["joints"]["prismatic joints"][last]["lower translation"] = castedJoint->GetLowerLimit()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["upper translation"] = castedJoint->GetUpperLimit()*pixelByMeter;
					root["joints"]["prismatic joints"][last]["enable motor"] = castedJoint->IsMotorEnabled();
					root["joints"]["prismatic joints"][last]["max motor force"] = castedJoint->GetMaxMotorForce();
					root["joints"]["prismatic joints"][last]["motor speed"] = castedJoint->GetMotorSpeed();
					root["joints"]["prismatic joints"][last]["reference angle"] = castedJoint->IsMotorEnabled();
					if(jointHasRole(joint, JointRole::BendingPower))
						root["joints"]["prismatic joints"][last]["roles"].append("bending power");
					break;
				}
					
				case e_distanceJoint:
				{
					break;
				}
					
				case e_pulleyJoint:
				{
					break;
				}
					
				case e_mouseJoint:
				{
					break;
				}
					
				case e_gearJoint:
				{
					break;
				}
					
				case e_wheelJoint:
				{
					break;
				}
					
				case e_weldJoint:
				{
					break;
				}
					
				case e_frictionJoint:
				{
					break;
				}
					
				case e_ropeJoint:
				{
					break;
				}
				
				case e_motorJoint:
				{
					break;
				}
				
				case e_unknownJoint:
				default:
				{
					break;
				}
			}
		}
		saveFileStream << writer.write(root);
		saveFileStream.close();
}

void GameState::initWorld(const std::string& file)
{
	const float scale{getContext().parameters.scale};
	const float pixelByMeter{getContext().parameters.pixelByMeter};//The pixel/meters scale at the maximum resolution, about 120.f
	TextureManager& texManager(getContext().textureManager);
	getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading save file"));
	try
	{
		//Parse the level data
		Json::Value root;//Will contains the root value after parsing.
		Json::Value model;
		Json::Reader reader;
		std::ifstream saveFile(file, std::ifstream::binary);
		std::ifstream modelSaveFile("resources/levels/model.json", std::ifstream::binary);
		if(!reader.parse(saveFile, root))//report to the user the failure and their locations in the document.
			throw std::runtime_error("\"" + file + "\" : " + reader.getFormattedErrorMessages());
		if(!reader.parse(modelSaveFile, model))
			throw std::runtime_error("\"resources/levels/model.json\" : " + reader.getFormattedErrorMessages());
		
		//SuperMegaMagic parsing of the save file from the model file
		parse(root, model, "root", "root");
		
		{
			const Json::Value time{root["time"]};
			getContext().systemManager.system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
			m_timeSpeed = time["speed"].asFloat();
			getContext().systemManager.system<AnimationsSystem>()->setTimeSpeed(m_timeSpeed);
		}
		const Json::Value level{root["level"]};
		
		//number of plans
		//must load it now in order to know how much plans can be defined
		m_numberOfPlans = level["number of plans"].asUInt();
		std::string identifier{level["identifier"].asString()};
		if(hasWhiteSpace(identifier))
			throw std::runtime_error("\"level.identifier\" value contains whitespaces.");
		m_levelIdentifier = identifier;
		m_referencePlan = level["reference plan"].asFloat();
		
		//box
		{
			const Json::Value levelBox{level["box"]};
			m_levelRect.width = levelBox["w"].asInt();
			m_levelRect.height = levelBox["h"].asInt();
			m_levelRect.top = levelBox["x"].asInt();
			m_levelRect.left = levelBox["y"].asInt();
		}
		
		getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
		
		//entities
		//Load entities before everything else allow to call the ScrollingSystem update after each sprite creation,
		//and so place sprite directly at the right position when loading the level, because the player is an entity,
		//and the scrolling system replace sprite according to the player position.
		if(root.isMember("entities"))
		{
			const Json::Value entities{root["entities"]};
			//First add all entities without add components, that's for component that need
			//that m_entities is complete to be assigned (such as InventoryComponent).
			for(std::string& entityName : entities.getMemberNames())
				m_entities.emplace(entityName, getContext().entityManager.create());
			
			for(std::string& entityName : entities.getMemberNames())
			{
				const Json::Value entity{entities[entityName]};
				
				if(entity.isMember("transforms"))
					deserialize(entity["transforms"], m_entities[entityName].assign<TransformComponent>());
				if(entity.isMember("sprites"))
					deserialize(entity["sprites"], m_entities[entityName].assign<SpriteComponent>(), texManager, paths[getContext().parameters.scaleIndex]);
				//Update the ScrollingSystem in order to directly display the sprite at the right position
				getContext().systemManager.update<ScrollingSystem>(sf::Time::Zero.asSeconds());
					
				//body
				if(entity.isMember("body"))
				{
					if(not entity.isMember("transforms"))
						throw std::runtime_error("\"root.entities." + entityName + ".body\" is defined but not \"root.entities." + entityName + ".transforms\"");
					else
					{
						try
						{
							deserialize(entity["body"], m_entities[entityName].assign<BodyComponent>(), m_entities[entityName].component<TransformComponent>(), getContext().world, pixelByMeter);
							//Assign user data to every body of the entity
							for(auto& bodyPair : m_entities[entityName].component<BodyComponent>()->bodies)
								bodyPair.second->SetUserData(&m_entities[entityName]);
						}
						catch(std::runtime_error& e)
						{
							std::cerr << "Unable to parse \"root.entities." + entityName + ".body\" : " << e.what() << std::endl;
						}
					}
				}
				
				//spritesheet animations
				if(entity.isMember("spritesheet animations"))
				{
					
					if(not entity.isMember("sprites"))
						throw std::runtime_error("\"root.entities." + entityName + ".spritesheet animations\" is defined but not \"root.entities." + entityName + ".sprites\"");
					else
					{
						try
						{
							deserialize(entity["spritesheet animations"], m_entities[entityName].assign<AnimationsComponent<SpriteSheetAnimation>>(), m_entities[entityName].component<SpriteComponent>(), getContext());
						}
						catch(std::runtime_error& e)
						{
							std::cerr << "Unable to parse \"root.entities." + entityName + ".spritesheet animations\" : " << e.what() << std::endl;
						}
					}
				}
				
				//Update the AnimationSystem in order to don't show the whole tileset of every entity on the screen when loading the level
				getContext().systemManager.update<AnimationsSystem>(sf::Time::Zero.asSeconds());
				
				if(entity.isMember("inventory"))
					deserialize(entity["inventory"], m_entities[entityName].assign<InventoryComponent>(), m_entities);
				if(entity.isMember("quiver"))
					deserialize(entity["quiver"], m_entities[entityName].assign<QuiverComponent>(), m_entities);
				if(entity.isMember("categories"))
					deserialize(entity["categories"], m_entities[entityName].assign<CategoryComponent>());
				if(entity.isMember("walk"))
					deserialize(entity["walk"], m_entities[entityName].assign<WalkComponent>());
				if(entity.isMember("jump"))
					deserialize(entity["jump"], m_entities[entityName].assign<JumpComponent>());
				if(entity.isMember("bend"))
					deserialize(entity["bend"], m_entities[entityName].assign<BendComponent>());
				if(entity.isMember("health"))
					deserialize(entity["health"], m_entities[entityName].assign<HealthComponent>());
				if(entity.isMember("stamina"))
					deserialize(entity["stamina"], m_entities[entityName].assign<StaminaComponent>());
				if(entity.isMember("direction"))
					deserialize(entity["direction"], m_entities[entityName].assign<DirectionComponent>());
				if(entity.isMember("fall"))
					deserialize(entity["fall"], m_entities[entityName].assign<FallComponent>());
				if(entity.isMember("wind friction"))
					deserialize(entity["wind friction"], m_entities[entityName].assign<WindFrictionComponent>());
			}
		}
		
		if(not getContext().parameters.debugMode)
		{
			//level
			//for each group of replaces
			for(std::string groupOfReplacesName : level["replaces"].getMemberNames())
			{
				//Filename of the image to load
				std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
				//Path of the image to load
				std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				
				const Json::Value groupOfReplaces{level["replaces"][groupOfReplacesName]};
				//Vector that will be added to m_sceneEntitiesData
				std::vector<SceneReplaces> planData;
				//For each image frame in the group of replaces
				for(Json::ArrayIndex i{0}; i < groupOfReplaces.size(); i++)
				{
					const Json::Value image{groupOfReplaces[i]};
					const Json::Value origin{image["origin"]};
					
					//Coordinates of the original image
					sf::IntRect originRect;
					originRect.left = static_cast<int>(origin["x"].asInt()*getContext().parameters.scale);
					originRect.top = static_cast<int>(origin["y"].asInt()*getContext().parameters.scale);
					originRect.width = static_cast<int>(origin["w"].asInt()*getContext().parameters.scale);
					originRect.height = static_cast<int>(origin["h"].asInt()*getContext().parameters.scale);
					
					SceneReplaces replacesData;
					//Directly take the data from the json value in order to keep the not scaled size
					replacesData.origin = {origin["x"].asInt(), origin["y"].asInt(), origin["w"].asInt(), origin["h"].asInt()};
					
					//Load the texture
					//Identifier of the texture, in format "level_plan_texture"
					std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
					//If the texture is not alreday loaded (first loading of the level)
					if(not texManager.isLoaded(textureIdentifier))
					{
						getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading plan") + L" " + std::wstring(groupOfReplacesName.begin(), groupOfReplacesName.end()));
						texManager.load<sf::IntRect>(textureIdentifier, path, originRect);
					}
					//Replaces
					const Json::Value replaces{image["replaces"]};
					//For each replacing of the image
					for(Json::ArrayIndex j{0}; j < replaces.size(); j++)
					{
						const Json::Value replace{replaces[j]};
						//Identifier of the entity, in format "level_plan_texture_replace"
						std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(j)};
						//Position where the frame should be replaced
						Transform replaceTransform;
						replaceTransform.x = replace["x"].asFloat()*getContext().parameters.scale;
						replaceTransform.y = replace["y"].asFloat()*getContext().parameters.scale;
						replaceTransform.z = replace["z"].asFloat();
						replaceTransform.angle = replace["angle"].asFloat();
						
						//Directly take the data from the json value in order to keep the not scaled size
						replacesData.replaces.push_back({replace["x"].asFloat(), replace["y"].asFloat(), replace["z"].asFloat(), replace["angle"].asFloat()});
						
						//Create an entity
						m_sceneEntities.emplace(replaceIdentifier, getContext().entityManager.create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite replaceSpr(texManager.get(textureIdentifier));
						replaceSpr.setPosition(replaceTransform.x, replaceTransform.y);
						replaceSpr.setRotation(replaceTransform.angle);
						std::map<std::string, sf::Sprite> sprites{{"main", replaceSpr}};
						std::map<std::string, Transform> transforms{{"main", replaceTransform}};
						SpriteComponent::Handle sprComp{m_sceneEntities[replaceIdentifier].assign<SpriteComponent>()};
						sprComp->sprites = sprites;
						TransformComponent::Handle trsfComp{m_sceneEntities[replaceIdentifier].assign<TransformComponent>()};
						trsfComp->transforms = transforms;
						CategoryComponent::Handle catComp{m_sceneEntities[replaceIdentifier].assign<CategoryComponent>()};
						catComp->category = Category::Scene;
						//Update the ScrollingSystem in order to directly display the sprite at the right position
						getContext().systemManager.update<ScrollingSystem>(sf::Time::Zero.asSeconds());
					}
					planData.push_back(replacesData);
				}
				m_sceneEntitiesData.emplace(groupOfReplacesName, planData);
			}
			//Load all the image in multiples chunks
			for(unsigned int i{0}; i < m_numberOfPlans; ++i)
			{
				std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
				std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				unsigned int chunkSize{sf::Texture::getMaximumSize()};
				//The length of the plan, relatively to the reference.
				unsigned int planLength{static_cast<unsigned int>((m_levelRect.width * pow(1.5, m_referencePlan - i))*getContext().parameters.scale)};
				//Number of chunks to load in this plan
				unsigned int numberOfChunks{(planLength/chunkSize)+1};
				
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
						getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading plan") + L" " + std::to_wstring(i+1));
						texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, m_levelRect.height*getContext().parameters.scale));
					}
					//Create an entity
					m_sceneEntities.emplace(textureIdentifier, getContext().entityManager.create());
					//Create a sprite with the loaded texture
					//Assign the sprite to the entity
					sf::Sprite chunkSpr(texManager.get(textureIdentifier));
					chunkSpr.setPosition(j*chunkSize, 0);
					std::map<std::string, sf::Sprite> sprites{{"main", chunkSpr}};
					std::map<std::string, Transform> transforms{{"main", {static_cast<float>(j*chunkSize), 0, static_cast<float>(i), 0}}};
					SpriteComponent::Handle sprComp{m_sceneEntities[textureIdentifier].assign<SpriteComponent>()};
					sprComp->sprites = sprites;
					TransformComponent::Handle trsfComp{m_sceneEntities[textureIdentifier].assign<TransformComponent>()};
					trsfComp->transforms = transforms;
					CategoryComponent::Handle catComp{m_sceneEntities[textureIdentifier].assign<CategoryComponent>()};
					catComp->category = Category::Scene;
					//Update the ScrollingSystem in order to directly display the sprite at the right position
					getContext().systemManager.update<ScrollingSystem>(sf::Time::Zero.asSeconds());
				}
			}
		}
		
		//joints
		if(root.isMember("joints"))
		{
			const Json::Value joints{root["joints"]};
			if(joints.isMember("revolute joints"))
			{
				const Json::Value revoluteJoints{joints["revolute joints"]};
				for(Json::ArrayIndex i{0}; i < revoluteJoints.size(); ++i)
				{
					const Json::Value joint{revoluteJoints[i]};
					std::string entityA{joint["entity A"].asString()};
					std::string entityB{joint["entity B"].asString()};
					std::string partA{joint["part A"].asString()};
					std::string partB{joint["part B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have some bodies
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});
					//Assert that that entities have the givens parts
					requireValues(root["entities"][entityA]["body"], "entities." + entityA + ".body", {{partA, Json::objectValue}});
					requireValues(root["entities"][entityB]["body"], "entities." + entityB + ".body", {{partB, Json::objectValue}});

					b2RevoluteJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->bodies[partA];
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->bodies[partB];
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.lowerAngle = (joint["lower angle"].asFloat() * b2_pi) / 180.f;
					jointDef.upperAngle = (joint["upper angle"].asFloat() * b2_pi) / 180.f;
					jointDef.enableLimit = joint["enable limit"].asBool();
					jointDef.maxMotorTorque = joint["max motor torque"].asFloat();
					jointDef.enableMotor = joint["enable motor"].asBool();
					
					//roles
					const Json::Value roles{joint["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
						if(roles[j].asString() == "bending angle")
							//Add the role to the definition
							jointDef.userData = add<unsigned int>(jointDef.userData, static_cast<unsigned int>(JointRole::BendingAngle));
					
					getContext().world.CreateJoint(&jointDef);
				}
			}
			if(joints.isMember("distance joints"))
			{
				const Json::Value distanceJoints{joints["distance joints"]};
				for(Json::ArrayIndex i{0}; i < distanceJoints.size(); ++i)
				{
					const Json::Value joint{distanceJoints[i]};
				}
			}
			if(joints.isMember("friction joints"))
			{
				const Json::Value frictionJoints{joints["friction joints"]};
				for(Json::ArrayIndex i{0}; i < frictionJoints.size(); ++i)
				{
					const Json::Value joint{frictionJoints[i]};
				}
			}
			if(joints.isMember("gear joints"))
			{
				const Json::Value gearJoints{joints["gear joints"]};
				for(Json::ArrayIndex i{0}; i < gearJoints.size(); ++i)
				{
					const Json::Value joint{gearJoints[i]};
				}
			}
			if(joints.isMember("motor joints"))
			{
				const Json::Value motorJoints{joints["motor joints"]};
				for(Json::ArrayIndex i{0}; i < motorJoints.size(); ++i)
				{
					const Json::Value joint{motorJoints[i]};
				}
			}
			if(joints.isMember("prismatic joints"))
			{
				const Json::Value prismaticJoints{joints["prismatic joints"]};
				for(Json::ArrayIndex i{0}; i < prismaticJoints.size(); ++i)
				{
					const Json::Value joint{prismaticJoints[i]};
					std::string entityA{joint["entity A"].asString()};
					std::string entityB{joint["entity B"].asString()};
					std::string partA{joint["part A"].asString()};
					std::string partB{joint["part B"].asString()};
					//Assert that somes entities exists
					requireValues(root, "root", {{"entities", Json::objectValue}});
					//Assert that the given entities exist
					requireValues(root["entities"], "entities", {{entityA, Json::objectValue}, {entityB, Json::objectValue}});
					//Assert that that entities have some bodies
					requireValues(root["entities"][entityA], "entities." + entityA, {{"body", Json::objectValue}});
					requireValues(root["entities"][entityB], "entities." + entityB, {{"body", Json::objectValue}});
					//Assert that that entities have the givens parts
					requireValues(root["entities"][entityA]["body"], "entities." + entityA + ".body", {{partA, Json::objectValue}});
					requireValues(root["entities"][entityB]["body"], "entities." + entityB + ".body", {{partB, Json::objectValue}});

					b2PrismaticJointDef jointDef;
					jointDef.bodyA = m_entities[entityA].component<BodyComponent>()->bodies[partA];
					jointDef.bodyB = m_entities[entityB].component<BodyComponent>()->bodies[partB];
					jointDef.localAnchorA.x = joint["local anchor A"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorA.y = joint["local anchor A"]["y"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.x = joint["local anchor B"]["x"].asFloat()/pixelByMeter;
					jointDef.localAnchorB.y = joint["local anchor B"]["y"].asFloat()/pixelByMeter;
					jointDef.localAxisA.x = joint["local axis A"]["x"].asFloat();
					jointDef.localAxisA.y = joint["local axis A"]["y"].asFloat();
					jointDef.lowerTranslation = joint["lower translation"].asFloat()/pixelByMeter;
					jointDef.upperTranslation = joint["upper translation"].asFloat()/pixelByMeter;
					jointDef.enableLimit = joint["enable limit"].asBool();
					jointDef.maxMotorForce = joint["max motor force"].asFloat();
					jointDef.motorSpeed = joint["motor speed"].asFloat()/pixelByMeter;
					jointDef.enableMotor = joint["enable motor"].asBool();
					jointDef.referenceAngle = (joint["reference angle"].asFloat() * b2_pi) / 180.f;
					
					//roles
					const Json::Value roles{joint["roles"]};
					for(Json::ArrayIndex j{0}; j < roles.size(); ++j)
						if(roles[j].asString() == "bending power")
							//Add the role to the definition
							jointDef.userData = add<unsigned int>(jointDef.userData, static_cast<unsigned int>(JointRole::BendingPower));
				}
			}
			if(joints.isMember("pulley joints"))
			{
				const Json::Value pulleyJoints{joints["pulley joints"]};
				for(Json::ArrayIndex i{0}; i < pulleyJoints.size(); ++i)
				{
					const Json::Value joint{pulleyJoints[i]};
				}
			}
			if(joints.isMember("rope joints"))
			{
				const Json::Value ropeJoints{joints["rope joints"]};
				for(Json::ArrayIndex i{0}; i < ropeJoints.size(); ++i)
				{
					const Json::Value joint{ropeJoints[i]};
				}
			}
			if(joints.isMember("weld joints"))
			{
				const Json::Value weldJoints{joints["weld joints"]};
				for(Json::ArrayIndex i{0}; i < weldJoints.size(); ++i)
				{
					const Json::Value joint{weldJoints[i]};
				}
			}
			if(joints.isMember("wheel joints"))
			{
				const Json::Value wheelJoints{joints["wheel joints"]};
				for(Json::ArrayIndex i{0}; i < wheelJoints.size(); ++i)
				{
					const Json::Value joint{wheelJoints[i]};
				}
			}
		}
		
		//Load the day sky
		const std::string dayIdentifier{"day sky"};
		sf::Vector2f position{1920.f/2.f, 1080.f};
		sf::Vector2f origin{2900.f/2.f, 2900.f/2.f};
		if(not texManager.isLoaded(dayIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading day sky"));
			texManager.load(dayIdentifier, paths[getContext().parameters.scaleIndex] + "day.png");
		}
		//Create a sprite with the loaded texture
		sf::Sprite daySpr(texManager.get(dayIdentifier));
		//Assign origin of the sprite to the center of the day image
		daySpr.setOrigin(origin*scale);
		
		//Load the night sky
		const std::string nightIdentifier{"night sky"};
		if(not texManager.isLoaded(nightIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading night sky"));
			texManager.load(nightIdentifier, paths[getContext().parameters.scaleIndex] + "night.png");
		}
		//Create a sprite with the loaded texture
		sf::Sprite nightSpr(texManager.get(nightIdentifier));
		//Assign origin of the sprite to the center of the night image
		nightSpr.setOrigin(origin*scale);
		
		//Create an entity
		m_sceneEntities.emplace("sky", getContext().entityManager.create());
		//Assign the sprites to the entity, and set its z-ordinates to positive infinity
		std::map<std::string, sf::Sprite> skySprites{{"day", daySpr}, {"night", nightSpr}};
		std::map<std::string, Transform> skyTransforms{{"day", {position.x, position.y, std::numeric_limits<float>::infinity(), 0}},
														{"night", {position.x, position.y, std::numeric_limits<float>::infinity(), 0}}};
		SpriteComponent::Handle sprComp{m_sceneEntities["sky"].assign<SpriteComponent>()};
		sprComp->sprites = skySprites;
		TransformComponent::Handle trsfComp{m_sceneEntities["sky"].assign<TransformComponent>()};
		trsfComp->transforms = skyTransforms;
		CategoryComponent::Handle catComp{m_sceneEntities["sky"].assign<CategoryComponent>()};
		catComp->category = Category::Scene;
		AnimationsComponent<SkyAnimation>::Handle skyAnimationsComp{m_sceneEntities["sky"].assign<AnimationsComponent<SkyAnimation>>()};
		skyAnimationsComp->animationsManagers.emplace("main", AnimationsManager<SkyAnimation>());
		//Add the animation, this is a sky animation, the importance is equal to zero, the duration is 600 seconds (1 day), and it loops.
		skyAnimationsComp->animationsManagers["main"].addAnimation("day/night cycle", SkyAnimation(m_sceneEntities["sky"]), 0, sf::seconds(600), true);
		double daySeconds{remainder(getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds(), 600)};
		skyAnimationsComp->animationsManagers["main"].setProgress("day/night cycle", daySeconds/600.f);
		skyAnimationsComp->animationsManagers["main"].play("day/night cycle");
		
		getContext().player.handleInitialInputState(getContext().pendingChanges.commandQueue);
		getContext().world.SetContactListener(&m_contactListener);
		getContext().world.SetContactFilter(&m_contactFilter);
		requestStackPop();
		requestStackPush(States::HUD);
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
		requestStackPop();
	}
}
