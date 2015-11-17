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
#include <TheLostGirl/states/PauseState.h>
#include <TheLostGirl/states/HUDState.h>
#include <TheLostGirl/states/MainMenuState.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
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

//FIXME Bad func call quand on on fait un scroll souris pendant le GameState
//TODO Statestack::Context as global variable
//TODO Mettre ArrowComponent::local(Friction|Stick)Point en pixels dans la sauvegarde
//TODO Finir la fonction flip().
//TODO Sauvegarde en fonction des includes.
//TODO Retirer la fleche pendant le CAC.
//TODO Sound manager.
//TODO Faire un système multi texture pour une même entité (soit chunker une grande image, soit faire plusieurs images).
//TODO CAC: statique : deux coups; en avancant: ??? ; en tombant: un puissant; en sautant : un faible; roulade : ???; en reculant : ???; en sneak : prise (egorgement/ippo sonage)

GameState::GameState(std::string file):
	m_contactListener(),
	m_timeSpeed{1.f},
	m_loading{true},
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 1920, 1080}
{
	Context::eventManager->subscribe<ParametersChange>(*this);
	m_threadLoad = std::thread(&GameState::initWorld, this, file);
}

GameState::~GameState()
{
	saveWorld("resources/levels/check.json");
	clear();
	if(m_threadLoad.joinable())
		m_threadLoad.join();
}

void GameState::draw()
{
	if(not m_loading)
	{
		Context::systemManager->update<RenderSystem>(0.f);
		//The drag and drop system draw a line on the screen, so we have to update it here
		Context::systemManager->update<DragAndDropSystem>(0.f);
	}
}

bool GameState::update(sf::Time elapsedTime)
{
	Context::systemManager->update<ScriptsSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<PendingChangesSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<AnimationsSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<PhysicsSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<ScrollingSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<StatsSystem>(elapsedTime.asSeconds());
	Context::systemManager->update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if((event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		or event.type == sf::Event::LostFocus)
		requestStackPush<PauseState>();

	Context::player->handleEvent(event);
	//Update the drag and drop state
	const bool isDragAndDropActive{Context::player->isActived(Player::Action::Bend)};
	Context::systemManager->system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);
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
		root["entities"] = Json::ValueType::objectValue;
		Serializer s(m_entities, root["entities"]);
		for(auto& entity : m_entities)
		{
			s.serialize<BodyComponent>(entity.first, "body");
			s.serialize<SpriteComponent>(entity.first, "sprite");
			s.serialize<TransformComponent>(entity.first, "transform");
			s.serialize<InventoryComponent>(entity.first, "inventory");
			s.serialize<ArcherComponent>(entity.first, "archer");
			s.serialize<AnimationsComponent<SpriteSheetAnimation>>(entity.first, "spritesheed animations");
			s.serialize<DirectionComponent>(entity.first, "direction");
			s.serialize<CategoryComponent>(entity.first, "categories");
			s.serialize<FallComponent>(entity.first, "fall");
			s.serialize<WalkComponent>(entity.first, "walk");
			s.serialize<JumpComponent>(entity.first, "jump");
			s.serialize<HealthComponent>(entity.first, "healt");
			s.serialize<StaminaComponent>(entity.first, "stamina");
			s.serialize<ArrowComponent>(entity.first, "arrow");
			s.serialize<HardnessComponent>(entity.first, "hardness");
			s.serialize<ScriptsComponent>(entity.first, "scripts");
			s.serialize<DetectionRangeComponent>(entity.first, "detection range");
			s.serialize<DeathComponent>(entity.first, "death");
			s.serialize<NameComponent>(entity.first, "name");
			s.serialize<HandToHandComponent>(entity.first, "hand to hand");
			s.serialize<ActorComponent>(entity.first, "actor");
			s.serialize<ItemComponent>(entity.first, "item");
			s.serialize<HoldItemComponent>(entity.first, "hold item");
			s.serialize<ArticuledArmsComponent>(entity.first, "articuled arms");
			s.serialize<BowComponent>(entity.first, "bow");
			s.serialize<QuiverComponent>(entity.first, "quiver");
			//End serialize
		}

		//time
		root["time"]["speed"] = m_timeSpeed;
		root["time"]["date"] = Context::systemManager->system<TimeSystem>()->getRealTime().asSeconds();

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
	Context::eventManager->emit<LoadingStateChange>("Loading save file");
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
		Context::systemManager->system<TimeSystem>()->setTotalTime(sf::seconds(time["date"].asFloat()));
		m_timeSpeed = time["speed"].asFloat();
		Context::systemManager->system<AnimationsSystem>()->setTimeSpeed(m_timeSpeed);

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

		Context::systemManager->system<ScrollingSystem>()->setLevelData(m_levelRect, m_referencePlan);
		Json::Value genericEntities;
		if(root.isMember("import"))
		{
			const Json::Value includes{root["import"]};
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
			Context::eventManager->emit<LoadingStateChange>("Loading entities");
			Json::Value jsonEntities{root["entities"]};
			Serializer s(m_entities, jsonEntities);
			std::set<std::string> alreadyDeserialized;
			std::function<void(const std::string&)> deserializerLambda = [&](const std::string& entityName)
			{
				if(alreadyDeserialized.count(entityName) > 0)
					return;

				std::cout << "Deserializing " << entityName << std::endl;
				entityx::Entity& entity{m_entities.emplace(entityName, Context::entityManager->create()).first->second};
				Json::Value& jsonEntity{jsonEntities[entityName]};

				//If the entity is derivated from a base entities, add json values of the base to the json entity
				if(jsonEntity.isMember("base"))
				{
					const std::string baseName{jsonEntity["base"].asString()};
					if(not TEST(genericEntities.isMember(baseName)))
						throw std::runtime_error("\"" + baseName + "\" is not a valid base.");
					const Json::Value base{genericEntities[baseName]};
					for(const std::string& componentName : base.getMemberNames())
						jsonEntity[componentName] = base[componentName];
				}

				//Deserialize first all dependecies of this entity
				Json::Value dependencies{jsonEntity["dependencies"]};
				for(Json::ArrayIndex i{0}; i < dependencies.size(); ++i)
					if(TEST(jsonEntities.isMember(dependencies[i].asString())))
						deserializerLambda(dependencies[i].asString());
					else
						throw std::runtime_error("\"" + dependencies[i].asString() + "\" is not a valid dependecy.");

				s.deserialize<TransformComponent>(entityName, "transform");
				s.deserialize<BodyComponent>(entityName, "body");
				BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
				if(bodyComponent)
					bodyComponent->body->SetUserData(&entity);
				s.deserialize<SpriteComponent>(entityName, "sprite");
				s.deserialize<AnimationsComponent<SpriteSheetAnimation>>(entityName, "spritesheet animations");
				s.deserialize<InventoryComponent>(entityName, "inventory");
				s.deserialize<ArcherComponent>(entityName, "archer");
				s.deserialize<CategoryComponent>(entityName, "categories");
				s.deserialize<WalkComponent>(entityName, "walk");
				s.deserialize<JumpComponent>(entityName, "jump");
				s.deserialize<HealthComponent>(entityName, "health");
				s.deserialize<StaminaComponent>(entityName, "stamina");
				s.deserialize<DirectionComponent>(entityName, "direction");
				s.deserialize<FallComponent>(entityName, "fall");
				s.deserialize<ArrowComponent>(entityName, "arrow");
				s.deserialize<HardnessComponent>(entityName, "hardness");
				s.deserialize<ScriptsComponent>(entityName, "scripts");
				s.deserialize<DetectionRangeComponent>(entityName, "detection range");
				s.deserialize<DeathComponent>(entityName, "death");
				s.deserialize<NameComponent>(entityName, "name");
				s.deserialize<HandToHandComponent>(entityName, "hand to hand");
				s.deserialize<ActorComponent>(entityName, "actor");
				s.deserialize<ItemComponent>(entityName, "item");
				s.deserialize<HoldItemComponent>(entityName, "hold item");
				s.deserialize<ArticuledArmsComponent>(entityName, "articuled arms");
				s.deserialize<BowComponent>(entityName, "bow");
				s.deserialize<QuiverComponent>(entityName, "quiver");
				//End deserialize
				alreadyDeserialized.insert(entityName);
			};
			for(const std::string& name : jsonEntities.getMemberNames())
				//Don't check if  jsonEntities[name] has been already deserialized, because
				//if there is only a few dependencies, there will be more useless check than useless lambda call
				deserializerLambda(name);
		}

		if(not Context::parameters->debugMode)
		{
			Context::eventManager->emit<LoadingStateChange>("Loading background");
			//level
			//for each group of replaces
			for(const std::string groupOfReplacesName : level["replaces"].getMemberNames())
			{
				//Filename of the image to load
				const std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
				//Path of the image to load
				const std::string path{paths[Context::parameters->scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};

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
					originRect.left = static_cast<int>(origin["x"].asFloat()*Context::parameters->scale);
					originRect.top = static_cast<int>(origin["y"].asFloat()*Context::parameters->scale);
					originRect.width = static_cast<int>(origin["w"].asFloat()*Context::parameters->scale);
					originRect.height = static_cast<int>(origin["h"].asFloat()*Context::parameters->scale);

					SceneReplaces replacesData;
					//Directly take the data from the json value in order to keep the not scaled size
					replacesData.origin = {origin["x"].asInt(), origin["y"].asInt(), origin["w"].asInt(), origin["h"].asInt()};

					//Load the texture
					//Identifier of the texture, in format "level_plan_texture"
					const std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
					Context::textureManager->load<sf::IntRect>(textureIdentifier, path, originRect);
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
						replaceTransform.x = replace["x"].asFloat()*Context::parameters->scale;
						replaceTransform.y = replace["y"].asFloat()*Context::parameters->scale;
						replaceTransform.z = replace["z"].asFloat();
						replaceTransform.angle = replace["angle"].asFloat();

						//Directly take the data from the json value in order to keep the not scaled size
						replacesData.replaces.push_back({replace["x"].asFloat(), replace["y"].asFloat(), replace["z"].asFloat(), replace["angle"].asFloat()});

						//Create an entity
						m_sceneEntities.emplace(replaceIdentifier, Context::entityManager->create());
						//Create a sprite with the loaded texture
						//Assign the sprite to the entity
						sf::Sprite replaceSpr(Context::textureManager->get(textureIdentifier));
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
				const std::string path{paths[Context::parameters->scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
				const int chunkSize{int(sf::Texture::getMaximumSize())};
				//The length of the plan, relatively to the reference.
				const int planLength{int(float(m_levelRect.width) * std::pow(1.5f, m_referencePlan - float(i)) * Context::parameters->scale)};
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
					Context::textureManager->load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, int(float(m_levelRect.height)*Context::parameters->scale)));
					//Create an entity
					m_sceneEntities.emplace(textureIdentifier, Context::entityManager->create());
					//Create a sprite with the loaded texture
					//Assign the sprite to the entity
					sf::Sprite chunkSpr(Context::textureManager->get(textureIdentifier));
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

		const float daySeconds{std::remainder(Context::systemManager->system<TimeSystem>()->getRealTime().asSeconds(), 600.f)};
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

		Context::systemManager->system<ScrollingSystem>()->searchPlayer();
		Context::systemManager->update<ScrollingSystem>(0.f);
		Context::systemManager->update<AnimationsSystem>(0.f);
		Context::player->handleInitialInputState();
		Context::world->SetContactListener(&m_contactListener);
		requestStackPop();
		Context::eventManager->emit<LoadingStateChange>("Loading HUD");
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
                Context::world->DestroyBody(entity.second.component<BodyComponent>()->body);

        entity.second.destroy();
    }
    for(auto& sceneEntity : m_sceneEntities)
        sceneEntity.second.destroy();
}
