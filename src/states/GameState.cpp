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
		
	saveWorld("resources/levels/save.json");
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
	for(auto& joint : m_joints)
		delete joint.definition;
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
				root["entities"][entity.first]["sprites"] = serialize(entity.second.component<SpriteComponent>(), getContext().textureManager);
			
			if(entity.second.has_component<TransformComponent>())
				root["entities"][entity.first]["transforms"] = serialize(entity.second.component<TransformComponent>(), scale);
			
			if(entity.second.has_component<AnimationsComponent<SpriteSheetAnimation>>())
				root["entities"][entity.first]["spritesheet animations"] = serialize(entity.second.component<AnimationsComponent<SpriteSheetAnimation>>());
			
			if(entity.second.has_component<DirectionComponent>())
				root["entities"][entity.first]["direction"] = serialize(entity.second.component<DirectionComponent>());
			
			if(entity.second.has_component<CategoryComponent>())
				root["entities"][entity.first]["categories"] = serialize(entity.second.component<CategoryComponent>());
			
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
				root["level"]["replaces"][planData.first][i]["origin"]["x"] = planData.second[i].origin.left/scale;
				root["level"]["replaces"][planData.first][i]["origin"]["y"] = planData.second[i].origin.top/scale;
				root["level"]["replaces"][planData.first][i]["origin"]["w"] = planData.second[i].origin.width/scale;
				root["level"]["replaces"][planData.first][i]["origin"]["h"] = planData.second[i].origin.height/scale;
				for(Json::ArrayIndex j{0}; j < planData.second[i].replaces.size(); ++j)
				{
					root["level"]["replaces"][planData.first][i]["replaces"][j]["x"] = planData.second[i].replaces[j].x/scale;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["y"] = planData.second[i].replaces[j].y/scale;
					root["level"]["replaces"][planData.first][i]["replaces"][j]["z"] = planData.second[i].replaces[j].z/scale;
				}
			}
		}
		
		//joints
		for(auto& joint : m_joints)
		{
			switch(joint.type)
			{
				case e_revoluteJoint:
				{
					b2RevoluteJointDef* castedJointDef = static_cast<b2RevoluteJointDef*>(joint.definition);
					root["joints"]["revolute joints"].append(Json::objectValue);
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["entity A"] = joint.entityA;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["entity B"] = joint.entityB;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["part A"] = joint.partA;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["part B"] = joint.partB;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["local anchor A"]["x"] = castedJointDef->localAnchorA.x/uniqScale;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["local anchor A"]["y"] = castedJointDef->localAnchorA.y/uniqScale;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["local anchor B"]["x"] = castedJointDef->localAnchorB.x/uniqScale;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["local anchor B"]["y"] = castedJointDef->localAnchorB.y/uniqScale;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["lower angle"] = (castedJointDef->lowerAngle / b2_pi) * 180.f;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["upper angle"] = (castedJointDef->upperAngle / b2_pi) * 180.f;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["enable limit"] = castedJointDef->enableLimit;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["max motor torque"] = castedJointDef->maxMotorTorque;
					root["joints"]["revolute joints"][root["joints"]["revolute joints"].size()-1]["enable motor"] = castedJointDef->enableMotor;
					break;
				}
				
				case e_prismaticJoint:
				{
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
	const float scale = getContext().parameters.scale;
	const float pixelScale = getContext().parameters.pixelScale;
	const float uniqScale = scale / pixelScale;//The pixel/meters scale at the maximum resolution, about 1.f/120.f
	TextureManager& texManager = getContext().textureManager;
	getContext().eventManager.emit<LoadingStateChange>(0, LangManager::tr("Loading save file"));
	try
	{
		//Parse the level data
		Json::Value root;//Will contains the root value after parsing.
		Json::Value model;
		Json::Reader reader;
		std::ifstream saveFile(file, std::ifstream::binary);
		std::ifstream modelSaveFile("resources/levels/model.json", std::ifstream::binary);
		if(!reader.parse(saveFile, root))//report to the user the failure and their locations in the document.
			throw std::runtime_error(reader.getFormattedErrorMessages());
		if(!reader.parse(modelSaveFile, model))
			throw std::runtime_error(reader.getFormattedErrorMessages());
		
		parse(root, model, "root", "root");
		
		//time
		{
			const Json::Value time = root["time"];
			
			//date
			getContext().systemManager.system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
			
			//speed
			m_timeSpeed = time["speed"].asFloat();
		}
		//level
		{
			const Json::Value level = root["level"];
			
			//number of plans
			//must load it now in order to know how much plans can be defined
			m_numberOfPlans = level["number of plans"].asUInt();
			
			//identifier
			std::string identifier = level["identifier"].asString();
			if(hasWhiteSpace(identifier))
				throw std::runtime_error("\"level.identifier\" value contains whitespaces.");
			m_levelIdentifier = identifier;
			
			//reference plan
			m_referencePlan = level["reference plan"].asFloat();
			
			//box
			{
				const Json::Value levelBox = level["box"];
				
				//width
				m_levelRect.width = levelBox["w"].asInt();
				
				//height
				m_levelRect.height = levelBox["h"].asInt();
				
				//x
				m_levelRect.top = levelBox["x"].asInt();
				
				//y
				m_levelRect.left = levelBox["y"].asInt();
			}
			
			//for each group of replaces
			for(std::string groupOfReplacesName : level["replaces"].getMemberNames())
			{
				//Filename of the image to load
				std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
				//Path of the image to load
				std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				
				const Json::Value groupOfReplaces = level["replaces"][groupOfReplacesName];
				//Vector that will be added to m_sceneEntitiesData
				std::vector<SceneReplaces> planData;
				//For each image frame in the group of replaces
				for(Json::ArrayIndex i{0}; i < groupOfReplaces.size(); i++)
				{
					const Json::Value image = groupOfReplaces[i];
					const Json::Value origin = image["origin"];
					
					//Coordinates of the original image
					sf::IntRect originRect;
					originRect.left = static_cast<unsigned int>(origin["x"].asUInt()*getContext().parameters.scale);
					originRect.top = static_cast<unsigned int>(origin["y"].asUInt()*getContext().parameters.scale);
					originRect.width = static_cast<unsigned int>(origin["w"].asUInt()*getContext().parameters.scale);
					originRect.height = static_cast<unsigned int>(origin["h"].asUInt()*getContext().parameters.scale);
					
					SceneReplaces replacesData;
					replacesData.origin = originRect;
					
					//Load the texture
					//Identifier of the texture, in format "level_plan_texture"
					std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
					//If the texture is not alreday loaded (first loading of the level)
					if(not texManager.isLoaded(textureIdentifier))
					{
						getContext().eventManager.emit<LoadingStateChange>(float(1*100)/float(m_numberOfPlans+1) + (float(i*100)/float(groupOfReplaces.size()*(m_numberOfPlans+1))), LangManager::tr("Loading plan") + L" " + std::wstring(groupOfReplacesName.begin(), groupOfReplacesName.end()));
						texManager.load<sf::IntRect>(textureIdentifier, path, originRect);
					}
					std::cout << "load replace " << textureIdentifier << std::endl;
					//Replaces
					const Json::Value replaces = image["replaces"];
					//For each replacing of the image
					for(Json::ArrayIndex j{0}; j < replaces.size(); j++)
					{
						const Json::Value replace = replaces[j];
						//Identifier of the entity, in format "level_plan_texture_replace"
						std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(j)};
						//Position where the frame should be replaced
						Transform replaceTransform;
						replaceTransform.x = replace["x"].asFloat()*getContext().parameters.scale;
						replaceTransform.y = replace["y"].asFloat()*getContext().parameters.scale;
						replaceTransform.z = replace["z"].asFloat();
						replaceTransform.angle = replace["angle"].asFloat();
						
						replacesData.replaces.push_back(replaceTransform);
						
						//Create an entity
						m_sceneEntities.emplace(replaceIdentifier, getContext().entityManager.create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite replaceSpr(texManager.get(textureIdentifier));
						replaceSpr.setPosition(replaceTransform.x, replaceTransform.y);
						replaceSpr.setRotation(replaceTransform.angle);
						std::map<std::string, sf::Sprite> sprites{{"main", replaceSpr}};
						std::map<std::string, Transform> transforms{{"main", replaceTransform}};
						SpriteComponent::Handle sprComp = m_sceneEntities[replaceIdentifier].assign<SpriteComponent>();
						sprComp->sprites = sprites;
						TransformComponent::Handle trsfComp = m_sceneEntities[replaceIdentifier].assign<TransformComponent>();
						trsfComp->transforms = transforms;
						CategoryComponent::Handle catComp = m_sceneEntities[replaceIdentifier].assign<CategoryComponent>();
						catComp->category = Category::Scene;
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
				unsigned int planLength = (m_levelRect.width * pow(1.5, m_referencePlan - i))*getContext().parameters.scale;
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
						getContext().eventManager.emit<LoadingStateChange>((float(i*100)/float(m_numberOfPlans+1)) + (float(j*100)/float(numberOfChunks*(m_numberOfPlans+1))), LangManager::tr("Loading plan") + L" " + std::to_wstring(i));
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
					SpriteComponent::Handle sprComp = m_sceneEntities[textureIdentifier].assign<SpriteComponent>();
					sprComp->sprites = sprites;
					TransformComponent::Handle trsfComp = m_sceneEntities[textureIdentifier].assign<TransformComponent>();
					trsfComp->transforms = transforms;
					CategoryComponent::Handle catComp = m_sceneEntities[textureIdentifier].assign<CategoryComponent>();
					catComp->category = Category::Scene;
				}
			}
		}
		
		//entities
		if(root.isMember("entities"))
		{
			const Json::Value entities = root["entities"];
			for(std::string& entityName : entities.getMemberNames())
			{
				const Json::Value entity = entities[entityName];
				m_entities.emplace(entityName, getContext().entityManager.create());
				
				//sprite
				if(entity.isMember("sprites"))
					deserialize(entity["sprites"], m_entities[entityName].assign<SpriteComponent>(), texManager);
				
				//tranforms
				if(entity.isMember("transforms"))
					deserialize(entity["transforms"], m_entities[entityName].assign<TransformComponent>(), scale);
					
				//body
				if(entity.isMember("body"))
				{
					deserialize(entity["body"], m_entities[entityName].assign<BodyComponent>(), getContext().world, uniqScale);
					//Assign user data to every body of the entity
					for(auto& bodyPair : m_entities[entityName].component<BodyComponent>()->bodies)
						bodyPair.second->SetUserData(&m_entities[entityName]);
				}
				//spritesheet animations
				if(entity.isMember("spritesheet animations"))
					deserialize(entity["spritesheet animations"], m_entities[entityName].assign<AnimationsComponent<SpriteSheetAnimation>>(), m_entities[entityName].component<SpriteComponent>(), getContext());
				
				//categories
				if(entity.isMember("categories"))
					deserialize(entity["categories"], m_entities[entityName].assign<CategoryComponent>());
				
				//actor ID
				if(entity.isMember("actor ID"))
					deserialize(entity["actor ID"], m_entities[entityName].assign<ActorIDComponent>());
				
				//walk
				if(entity.isMember("walk"))
					deserialize(entity["walk"], m_entities[entityName].assign<WalkComponent>());
				
				//jump
				if(entity.isMember("jump"))
					deserialize(entity["jump"], m_entities[entityName].assign<JumpComponent>());
				
				//bend
				if(entity.isMember("bend"))
					deserialize(entity["bend"], m_entities[entityName].assign<BendComponent>());
				
				//health
				if(entity.isMember("health"))
					deserialize(entity["health"], m_entities[entityName].assign<HealthComponent>());
				
				//stamina
				if(entity.isMember("stamina"))
					deserialize(entity["stamina"], m_entities[entityName].assign<StaminaComponent>());
				
				//direction
				if(entity.isMember("direction"))
					deserialize(entity["direction"], m_entities[entityName].assign<DirectionComponent>());
				
				//fall
				if(entity.isMember("fall"))
					deserialize(entity["fall"], m_entities[entityName].assign<FallComponent>());
			}
		}
		
		//joints
		if(root.isMember("joints"))
		{
			const Json::Value joints = root["joints"];
			if(joints.isMember("revolute joints"))
			{
				const Json::Value revoluteJoints = joints["revolute joints"];
				for(Json::ArrayIndex i{0}; i < revoluteJoints.size(); ++i)
				{
					const Json::Value joint = revoluteJoints[i];
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
			}
			if(joints.isMember("distance joints"))
			{
				const Json::Value distanceJoints = joints["distance joints"];
				for(Json::ArrayIndex i{0}; i < distanceJoints.size(); ++i)
				{
					const Json::Value joint = distanceJoints[i];
				}
			}
			if(joints.isMember("friction joints"))
			{
				const Json::Value frictionJoints = joints["friction joints"];
				for(Json::ArrayIndex i{0}; i < frictionJoints.size(); ++i)
				{
					const Json::Value joint = frictionJoints[i];
				}
			}
			if(joints.isMember("gear joints"))
			{
				const Json::Value gearJoints = joints["gear joints"];
				for(Json::ArrayIndex i{0}; i < gearJoints.size(); ++i)
				{
					const Json::Value joint = gearJoints[i];
				}
			}
			if(joints.isMember("motor joints"))
			{
				const Json::Value motorJoints = joints["motor joints"];
				for(Json::ArrayIndex i{0}; i < motorJoints.size(); ++i)
				{
					const Json::Value joint = motorJoints[i];
				}
			}
			if(joints.isMember("prismatic joints"))
			{
				const Json::Value prismaticJoints = joints["prismatic joints"];
				for(Json::ArrayIndex i{0}; i < prismaticJoints.size(); ++i)
				{
					const Json::Value joint = prismaticJoints[i];
				}
			}
			if(joints.isMember("pulley joints"))
			{
				const Json::Value pulleyJoints = joints["pulley joints"];
				for(Json::ArrayIndex i{0}; i < pulleyJoints.size(); ++i)
				{
					const Json::Value joint = pulleyJoints[i];
				}
			}
			if(joints.isMember("rope joints"))
			{
				const Json::Value ropeJoints = joints["rope joints"];
				for(Json::ArrayIndex i{0}; i < ropeJoints.size(); ++i)
				{
					const Json::Value joint = ropeJoints[i];
				}
			}
			if(joints.isMember("weld joints"))
			{
				const Json::Value weldJoints = joints["weld joints"];
				for(Json::ArrayIndex i{0}; i < weldJoints.size(); ++i)
				{
					const Json::Value joint = weldJoints[i];
				}
			}
			if(joints.isMember("wheel joints"))
			{
				const Json::Value wheelJoints = joints["wheel joints"];
				for(Json::ArrayIndex i{0}; i < wheelJoints.size(); ++i)
				{
					const Json::Value joint = wheelJoints[i];
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
		m_sceneEntities.emplace(dayIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite daySpr(texManager.get(dayIdentifier));
		//Assign origin of the sprite to the center of the day image
		daySpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		std::map<std::string, sf::Sprite> daySprites{{"main", daySpr}};
		std::map<std::string, Transform> dayTransforms{{"main", {position.x, position.y, std::numeric_limits<float>::infinity(), 0}}};
		SpriteComponent::Handle sprComp = m_sceneEntities[dayIdentifier].assign<SpriteComponent>();
		sprComp->sprites = daySprites;
		TransformComponent::Handle trsfComp = m_sceneEntities[dayIdentifier].assign<TransformComponent>();
		trsfComp->transforms = dayTransforms;
		CategoryComponent::Handle catComp = m_sceneEntities[dayIdentifier].assign<CategoryComponent>();
		catComp->category = Category::Scene;
		SkyComponent::Handle skyComp = m_sceneEntities[dayIdentifier].assign<SkyComponent>();
		skyComp->day = true;
	
		//Load the night sky
		const std::string nightIdentifier{"night sky"};
		if(not texManager.isLoaded(nightIdentifier))
		{
			getContext().eventManager.emit<LoadingStateChange>(float(m_numberOfPlans*100 + 50)/float(m_numberOfPlans+1), LangManager::tr("Loading night sky"));
			texManager.load(nightIdentifier, paths[getContext().parameters.scaleIndex] + "night.png");
		}
		//Create an entity
		m_sceneEntities.emplace(nightIdentifier, getContext().entityManager.create());
		//Create a sprite with the loaded texture
		sf::Sprite nightSpr(texManager.get(nightIdentifier));
		//Assign origin of the sprite to the center of the night image
		nightSpr.setOrigin(origin);
		//Assign the sprite to the entity, and set its z-ordinate to positive infinity
		std::map<std::string, sf::Sprite> nightSprites{{"main", nightSpr}};
		std::map<std::string, Transform> nightTransforms{{"main", {position.x, position.y, std::numeric_limits<float>::infinity(), 0}}};
		sprComp = m_sceneEntities[nightIdentifier].assign<SpriteComponent>();
		sprComp->sprites = nightSprites;
		trsfComp = m_sceneEntities[nightIdentifier].assign<TransformComponent>();
		trsfComp->transforms = nightTransforms;
		catComp = m_sceneEntities[nightIdentifier].assign<CategoryComponent>();
		catComp->category = Category::Scene;
		skyComp = m_sceneEntities[nightIdentifier].assign<SkyComponent>();
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
	getContext().world.SetContactFilter(&m_contactFilter);
	getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
	requestStackPop();
	requestStackPush(States::HUD);
	CategoryComponent::Handle categoryComponent;
}
