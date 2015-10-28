#include <fstream>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/Joints/b2DistanceJoint.h>
#include <Box2D/Dynamics/Joints/b2PulleyJoint.h>
#include <Box2D/Dynamics/Joints/b2WheelJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/Joints/b2FrictionJoint.h>
#include <Box2D/Dynamics/Joints/b2RopeJoint.h>
#include <Box2D/Dynamics/Joints/b2MotorJoint.h>
#include <entityx/entityx.h>
#include <dist/json/json.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/states/PauseState.h>
#include <TheLostGirl/states/HUDState.h>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/SkyAnimation.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/serialization.h>
#include <TheLostGirl/states/GameState.h>

//TODO Finir la fonction flip().
//TODO Sauvegarde en fonction des includes.
//TODO Retirer la fleche pendant le CAC.
//TODO Sound manager.
//TODO Faire un système multi texture pour une même partie d'entité (soit chunker une grande image, soit faire plusieurs images).

// À discuter, à propos du gameplay :
//TODO Component pour la position de combat (parade, attaque, tir, ...).
//TODO Differents CAC en fonction du déplacement.

GameState::GameState(StateStack& stack, std::string file) :
	State(stack),
	m_contactListener(getContext()),
	m_timeSpeed{1.f},
	m_loading{true},
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 1920, 1080}
{
	getContext().eventManager.subscribe<ParametersChange>(*this);
	m_threadLoad = std::thread(&GameState::initWorld, this, file);
}

GameState::~GameState()
{
//	saveWorld("resources/levels/S.json");
	clear();
	if(m_threadLoad.joinable())
		m_threadLoad.join();
}

void GameState::draw()
{
	if(not m_loading)
	{
		getContext().systemManager.update<RenderSystem>(0.f);
		//The drag and drop system draw a line on the screen, so we have to update it here
		getContext().systemManager.update<DragAndDropSystem>(0.f);
	}
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<ScriptsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<PendingChangesSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<PhysicsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ScrollingSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<StatsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if((event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		or event.type == sf::Event::LostFocus)
		requestStackPush<PauseState>();

	getContext().player.handleEvent(event);
	//Update the drag and drop state
	const bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);
	return false;
}

void GameState::receive(const ParametersChange&)
{
}

void GameState::saveWorld(const std::string& file)
{
		Json::Value root;//Will contains the root value after serializing.
		Json::StyledWriter writer;
		std::ofstream saveFileStream(file, std::ofstream::binary);
		for(auto& entity : m_entities)
		{
			Serializer s(getContext(), m_entities);
#define SERIALIZE(componentName, componentType)\
if(entity.second.has_component<componentType>())\
	root["entities"][entity.first][componentName] = s.serialize(entity.second.component<componentType>());
			SERIALIZE("body", BodyComponent)
			SERIALIZE("sprite", SpriteComponent)
			SERIALIZE("tranform", TransformComponent)
			SERIALIZE("inventory", InventoryComponent)
			SERIALIZE("archer", ArcherComponent)
			SERIALIZE("spritesheed animations", AnimationsComponent<SpriteSheetAnimation>)
			SERIALIZE("direction", DirectionComponent)
			SERIALIZE("categories", CategoryComponent)
			SERIALIZE("fall", FallComponent)
			SERIALIZE("walk", WalkComponent)
			SERIALIZE("jump", JumpComponent)
			SERIALIZE("healt", HealthComponent)
			SERIALIZE("stamina", StaminaComponent)
			SERIALIZE("arrow", ArrowComponent)
			SERIALIZE("hardness", HardnessComponent)
			SERIALIZE("scripts", ScriptsComponent)
			SERIALIZE("detection range", DetectionRangeComponent)
			SERIALIZE("death", DeathComponent)
			SERIALIZE("name", NameComponent)
			SERIALIZE("hand to hand", HandToHandComponent)
			SERIALIZE("actor", ActorComponent)
			SERIALIZE("hold item", HoldItemComponent)
			SERIALIZE("articuled arms", ArticuledArmsComponent)
			SERIALIZE("bow", BowComponent)
			SERIALIZE("quiver", QuiverComponent)
			//End serialize
#undef SERIALIZE
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
			for(unsigned int i{0}; i < planData.second.size(); ++i)
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

		saveFileStream << writer.write(root);
		saveFileStream.close();
}

void GameState::initWorld(const std::string& file)
{
	TextureManager& texManager(getContext().textureManager);
	getContext().eventManager.emit<LoadingStateChange>("Loading save file");
	try
	{
		//Parse the level data
		Json::Value root;//Will contains the root value after parsing.
		Json::Value model;
		Json::Reader reader;
		std::ifstream saveFile(file, std::ifstream::binary);
		std::ifstream modelSaveFile("resources/levels/model.json", std::ifstream::binary);
		if(not reader.parse(saveFile, root))//report to the user the failure and their locations in the document.
			throw std::runtime_error("save file \"" + file + "\" : " + reader.getFormattedErrorMessages());
		saveFile.close();
		if(not reader.parse(modelSaveFile, model))
			throw std::runtime_error("model file \"resources/levels/model.json\": " + reader.getFormattedErrorMessages());
		modelSaveFile.close();

		//SuperMegaMagic parsing of the save file from the model file
		parse(root, model, "root", "root");

		const Json::Value time{root["time"]};
		getContext().systemManager.system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
		m_timeSpeed = time["speed"].asFloat();
		getContext().systemManager.system<AnimationsSystem>()->setTimeSpeed(m_timeSpeed);

		const Json::Value level{root["level"]};
		//number of plans
		//must load it now in order to know how much plans can be defined
		m_numberOfPlans = level["number of plans"].asUInt();
		const std::string identifier{level["identifier"].asString()};
		if(hasWhiteSpace(identifier))
			throw std::runtime_error("\"level.identifier\" value contains whitespaces.");
		m_levelIdentifier = identifier;
		m_referencePlan = level["reference plan"].asFloat();

		//box
		const Json::Value levelBox{level["box"]};
		m_levelRect.width = levelBox["w"].asInt();
		m_levelRect.height = levelBox["h"].asInt();
		m_levelRect.top = levelBox["x"].asInt();
		m_levelRect.left = levelBox["y"].asInt();

		getContext().systemManager.system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
		Json::Value genericEntities;
		if(root.isMember("include"))
		{
			const Json::Value includes{root["include"]};
			for(Json::ArrayIndex i{0}; i < includes.size(); ++i)
			{
				//Parse the level data
				Json::Value includeRoot;//Will contains the root value after parsing.
				std::ifstream includeFile("resources/levels/" + includes[i].asString(), std::ifstream::binary);
				if(not reader.parse(includeFile, includeRoot))//report to the user the failure and their locations in the document.
					throw std::runtime_error("included file \"resources/levels/" + includes[i].asString() + "\" : " + reader.getFormattedErrorMessages());
				includeFile.close();

				//Parsing of the included file from the model file
				parse(includeRoot, model, "root", "root");
				//Add generic entities to others
				Json::Value currentGenericEntities{includeRoot["entities"]};
				for(const std::string& genericEntityName : currentGenericEntities.getMemberNames())
					genericEntities[genericEntityName] = currentGenericEntities[genericEntityName];
			}
		}

		//entities
		//Load entities before everything else allow to call the ScrollingSystem update after each sprite creation,
		//and so place sprite directly at the right position when loading the level, because the player is an entity,
		//and the scrolling system replace sprite according to the player position.
		if(root.isMember("entities"))
		{
			getContext().eventManager.emit<LoadingStateChange>("Loading entities");
			const Json::Value entities{root["entities"]};
			//First add all entities with some needed components, that's for component that need
			//that m_entities is complete and bodies already created to be assigned (such as InventoryComponent).
			for(const std::string& entityName : entities.getMemberNames())
			{
				Json::Value entity{entities[entityName]};
				//If the entity is derivated from a generic entities, add components of the base to the entity
				if(entity.isMember("base"))
				{
					if(not genericEntities.isMember(entity["base"].asString()))
						throw std::runtime_error("\"entities." + entityName + ".base\" value is not an existing base.");
					const Json::Value base{genericEntities[entity["base"].asString()]};
					for(const std::string& componentName : base.getMemberNames())
						entity[componentName] = base[componentName];
				}
				m_entities.emplace(entityName, getContext().entityManager.create());
				Serializer s(getContext(), m_entities);
				if(entity.isMember("transform"))
					s.deserialize(entity["transform"], m_entities[entityName].assign<TransformComponent>());
				if(entity.isMember("body"))
				{
					s.deserialize(entity["body"], m_entities[entityName].assign<BodyComponent>(), m_entities[entityName].component<TransformComponent>());
					//Assign user data to the body of the entity
					m_entities[entityName].component<BodyComponent>()->body->SetUserData(&m_entities[entityName]);
				}
			}

			for(const std::string& entityName : entities.getMemberNames())
			{
				Json::Value entity{entities[entityName]};
				//If the entity is derivated from a generic entities, add components of the base to the entity
				if(entity.isMember("base"))
				{
					const Json::Value base{genericEntities[entity["base"].asString()]};
					for(const std::string& componentName : base.getMemberNames())
						entity[componentName] = base[componentName];
				}
				//Assign to root the new entity
				root["entities"][entityName] = entity;

				Serializer s(getContext(), m_entities);
#define DESERIALIZE(componentName, componentType)\
if(entity.isMember(componentName))\
	s.deserialize(entity[componentName], m_entities[entityName].assign<componentType>());
#define DESERIALIZE_(componentName, componentType, dependency)\
if(entity.isMember(componentName))\
	s.deserialize(entity[componentName], m_entities[entityName].assign<componentType>(), m_entities[entityName].component<dependency>());
				DESERIALIZE("sprite", SpriteComponent)
				DESERIALIZE_("spritesheet animations", AnimationsComponent<SpriteSheetAnimation>, SpriteComponent)
				DESERIALIZE("inventory", InventoryComponent)
				DESERIALIZE_("archer", ArcherComponent, BodyComponent)
				DESERIALIZE("categories", CategoryComponent)
				DESERIALIZE("walk", WalkComponent)
				DESERIALIZE("jump", JumpComponent)
				DESERIALIZE("health", HealthComponent)
				DESERIALIZE("stamina", StaminaComponent)
				DESERIALIZE("direction", DirectionComponent)
				DESERIALIZE("fall", FallComponent)
				DESERIALIZE("arrow", ArrowComponent)
				DESERIALIZE("hardness", HardnessComponent)
				DESERIALIZE("scripts", ScriptsComponent)
				DESERIALIZE("detection range", DetectionRangeComponent)
				DESERIALIZE("death", DeathComponent)
				DESERIALIZE("name", NameComponent)
				DESERIALIZE("hand to hand", HandToHandComponent)
				DESERIALIZE("actor", ActorComponent)
				DESERIALIZE_("hold item", HoldItemComponent, BodyComponent)
				DESERIALIZE_("articuled arms", ArticuledArmsComponent, BodyComponent)
				DESERIALIZE_("bow", BowComponent, BodyComponent)
				DESERIALIZE_("quiver", QuiverComponent, BodyComponent)
				//End deserialize
#undef DESERIALIZE
			}
		}

		if(not getContext().parameters.debugMode)
		{
			getContext().eventManager.emit<LoadingStateChange>("Loading background");
			//level
			//for each group of replaces
			for(const std::string groupOfReplacesName : level["replaces"].getMemberNames())
			{
				//Filename of the image to load
				const std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
				//Path of the image to load
				const std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};

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
					originRect.left = static_cast<int>(origin["x"].asFloat()*getContext().parameters.scale);
					originRect.top = static_cast<int>(origin["y"].asFloat()*getContext().parameters.scale);
					originRect.width = static_cast<int>(origin["w"].asFloat()*getContext().parameters.scale);
					originRect.height = static_cast<int>(origin["h"].asFloat()*getContext().parameters.scale);

					SceneReplaces replacesData;
					//Directly take the data from the json value in order to keep the not scaled size
					replacesData.origin = {origin["x"].asInt(), origin["y"].asInt(), origin["w"].asInt(), origin["h"].asInt()};

					//Load the texture
					//Identifier of the texture, in format "level_plan_texture"
					const std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
					texManager.load<sf::IntRect>(textureIdentifier, path, originRect);
					//Replaces
					const Json::Value replaces{image["replaces"]};
					//For each replacing of the image
					for(Json::ArrayIndex j{0}; j < replaces.size(); j++)
					{
						const Json::Value replace{replaces[j]};
						//Identifier of the entity, in format "level_plan_texture_replace"
						const std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(j)};
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
						SpriteComponent::Handle sprComp{m_sceneEntities[replaceIdentifier].assign<SpriteComponent>()};
						sprComp->sprite = replaceSpr;
						TransformComponent::Handle trsfComp{m_sceneEntities[replaceIdentifier].assign<TransformComponent>()};
						trsfComp->transform = replaceTransform;
						CategoryComponent::Handle catComp{m_sceneEntities[replaceIdentifier].assign<CategoryComponent>()};
						catComp->category = Category::Scene;
					}
					planData.push_back(replacesData);
				}
				m_sceneEntitiesData.emplace(groupOfReplacesName, planData);
			}
			//Load all the image in multiples chunks
			for(unsigned int i{0}; i < m_numberOfPlans; ++i)
			{
				const std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
				const std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				const int chunkSize{int(sf::Texture::getMaximumSize())};
				//The length of the plan, relatively to the reference.
				const int planLength{int(float(m_levelRect.width) * std::pow(1.5f, m_referencePlan - float(i)) * getContext().parameters.scale)};
				//Number of chunks to load in this plan
				const int numberOfChunks{(planLength/chunkSize)+1};

				for(int j{0}; j < numberOfChunks; ++j)
				{
					//Identifier of the entity, in format "level_plan_chunk"
					const std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
					//Size of the chunk to load, may be truncated if we reach the end of the image.
					int currentChunkSize{chunkSize};
					if(j >= planLength/chunkSize)
						currentChunkSize = planLength - chunkSize*j;
					texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, int(float(m_levelRect.height)*getContext().parameters.scale)));
					//Create an entity
					m_sceneEntities.emplace(textureIdentifier, getContext().entityManager.create());
					//Create a sprite with the loaded texture
					//Assign the sprite to the entity
					sf::Sprite chunkSpr(texManager.get(textureIdentifier));
					chunkSpr.setPosition(float(j*chunkSize), 0);
					SpriteComponent::Handle sprComp{m_sceneEntities[textureIdentifier].assign<SpriteComponent>()};
					sprComp->sprite = chunkSpr;
					TransformComponent::Handle trsfComp{m_sceneEntities[textureIdentifier].assign<TransformComponent>()};
					trsfComp->transform = {static_cast<float>(j*chunkSize), 0, static_cast<float>(i), 0};
					CategoryComponent::Handle catComp{m_sceneEntities[textureIdentifier].assign<CategoryComponent>()};
					catComp->category = Category::Scene;
					//Update the ScrollingSystem in order to directly display the sprite at the right position
				}
			}
		}

		const float daySeconds{std::remainder(getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds(), 600.f)};
		//Add sky animations.
		if(m_entities.find("day sky") != m_entities.end() and m_entities["day sky"].valid())
		{
			AnimationsComponent<SkyAnimation>::Handle skyAnimationsComp{m_entities["day sky"].assign<AnimationsComponent<SkyAnimation>>()};
			//Add the animation, this is a sky animation, the importance is equal to zero, the duration is 600 seconds (1 day), and it loops.
			skyAnimationsComp->animationsManager.addAnimation("day/night cycle", SkyAnimation(m_entities["day sky"]), 0, sf::seconds(600), true);
			skyAnimationsComp->animationsManager.setProgress("day/night cycle", daySeconds/600.f);
			skyAnimationsComp->animationsManager.play("day/night cycle");
		}

		//Add sky animations.
		if(m_entities.find("night sky") != m_entities.end() and m_entities["night sky"].valid())
		{
			AnimationsComponent<SkyAnimation>::Handle skyAnimationsComp{m_entities["night sky"].assign<AnimationsComponent<SkyAnimation>>()};
			//Add the animation, this is a sky animation, the importance is equal to zero, the duration is 600 seconds (1 day), and it loops.
			skyAnimationsComp->animationsManager.addAnimation("day/night cycle", SkyAnimation(m_entities["night sky"]), 0, sf::seconds(600), true);
			skyAnimationsComp->animationsManager.setProgress("day/night cycle", daySeconds/600.f);
			skyAnimationsComp->animationsManager.play("day/night cycle");
		}

		getContext().systemManager.system<ScrollingSystem>()->searchPlayer(getContext().entityManager);
		getContext().systemManager.update<ScrollingSystem>(0.f);
		getContext().systemManager.update<AnimationsSystem>(0.f);
		getContext().player.handleInitialInputState();
		getContext().world.SetContactListener(&m_contactListener);
		requestStackPop();
		getContext().eventManager.emit<LoadingStateChange>("Loading HUD");
		requestStackPush<HUDState>();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		std::cerr << "Failed to load save file \"" << file << "\".\n";
		requestStackPop();
		requestStackPop();
		requestStackPush<MainMenuState>();
	}
		m_loading = false;
}

void GameState::clear()
{
    for(auto& entity : m_entities)
    {
        if(entity.second.has_component<BodyComponent>())
                getContext().world.DestroyBody(entity.second.component<BodyComponent>()->body);

        entity.second.destroy();
    }
    for(auto& sceneEntity : m_sceneEntities)
        sceneEntity.second.destroy();
}
