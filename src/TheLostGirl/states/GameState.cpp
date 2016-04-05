#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <dist/json/json.h>
#include <boost/filesystem.hpp>
#include <TheLostGirl/states/PauseState.hpp>
#include <TheLostGirl/states/HUDState.hpp>
#include <TheLostGirl/states/MainMenuState.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/Category.hpp>
#include <TheLostGirl/systems.hpp>
#include <TheLostGirl/ResourceManager.hpp>
#include <TheLostGirl/Player.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/SkyAnimation.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/Serializer.hpp>
#include <TheLostGirl/JsonHelper.hpp>
#include <TheLostGirl/states/GameState.hpp>

GameState::GameState(std::string file):
	m_timeSpeed{1.f},
	m_loading{true},
	m_savesDirectoryPath{"saves/"},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{{0.f, 0.f}, Context::getParameters().defaultViewSize}
{
	Context::getEventManager().subscribe<ParametersChange>(*this);
	m_threadLoad = std::thread(&GameState::initWorld, this, file);
}

GameState::~GameState()
{
	saveWorld(m_savesDirectoryPath + "player.json");
	clear();
	if(m_threadLoad.joinable())
		m_threadLoad.join();
}

void GameState::draw()
{
	if(not m_loading)
	{
		Context::getSystemManager().update<RenderSystem>(0.f);
		//The drag and drop system draw a line on the screen, so we have to update it here
		Context::getSystemManager().update<DragAndDropSystem>(0.f);
	}
}

bool GameState::update(sf::Time elapsedTime)
{
	Context::getSystemManager().update<ScriptsSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<PendingChangesSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<AnimationsSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<PhysicsSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<ScrollingSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<StatsSystem>(elapsedTime.asSeconds());
	Context::getSystemManager().update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if((event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		or event.type == sf::Event::LostFocus)
		requestStackPush<PauseState>();

	Context::getPlayer().handleEvent(event);
	//Update the drag and drop state
	const bool isDragAndDropActive{Context::getPlayer().isActived(Player::Action::Bend)};
	getSystem<DragAndDropSystem>().setDragAndDropActivation(isDragAndDropActive);
	return false;
}

void GameState::receive(const ParametersChange&)
{
}

Json::Value GameState::getJsonValueFromGame()
{
	Json::Value root;
	root["entities"] = Json::ValueType::objectValue;
	Serializer s(m_entities, root["entities"]);
	for(auto& entity : m_entities)
	{
		s.serialize<BodyComponent>(entity.first, "body");
		s.serialize<SpriteComponent>(entity.first, "sprite");
		s.serialize<TransformComponent>(entity.first, "transform");
		s.serialize<InventoryComponent>(entity.first, "inventory");
		s.serialize<ArcherComponent>(entity.first, "archer");
		s.serialize<AnimationsComponent<SpriteSheetAnimation>>(entity.first, "spritesheet animations");
		s.serialize<DirectionComponent>(entity.first, "direction");
		s.serialize<CategoryComponent>(entity.first, "categories");
		s.serialize<FallComponent>(entity.first, "fall");
		s.serialize<WalkComponent>(entity.first, "walk");
		s.serialize<JumpComponent>(entity.first, "jump");
		s.serialize<HealthComponent>(entity.first, "health");
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
	root["time"]["date"] = getSystem<TimeSystem>().getRealTime().asSeconds();

	//level data
	root["level data"]["identifier"] = m_levelIdentifier;
	root["level data"]["box"]["x"] = m_levelRect.left;
	root["level data"]["box"]["y"] = m_levelRect.top;
	root["level data"]["box"]["w"] = m_levelRect.width;
	root["level data"]["box"]["h"] = m_levelRect.height;
	root["level data"]["number of plans"] = m_numberOfPlans;
	root["level data"]["reference plan"] = m_referencePlan;

	//replaces
	for(auto& planData : m_sceneEntitiesData)
	{
		for(unsigned int i{0}; i < planData.second.size(); ++i)
		{
			root["level data"]["replaces"][planData.first][i]["origin"]["x"] = planData.second[i].origin.left;
			root["level data"]["replaces"][planData.first][i]["origin"]["y"] = planData.second[i].origin.top;
			root["level data"]["replaces"][planData.first][i]["origin"]["w"] = planData.second[i].origin.width;
			root["level data"]["replaces"][planData.first][i]["origin"]["h"] = planData.second[i].origin.height;
			for(Json::ArrayIndex j{0}; j < planData.second[i].replaces.size(); ++j)
			{
				root["level data"]["replaces"][planData.first][i]["replaces"][j]["x"] = planData.second[i].replaces[j].x;
				root["level data"]["replaces"][planData.first][i]["replaces"][j]["y"] = planData.second[i].replaces[j].y;
				root["level data"]["replaces"][planData.first][i]["replaces"][j]["z"] = planData.second[i].replaces[j].z;
				root["level data"]["replaces"][planData.first][i]["replaces"][j]["angle"] = planData.second[i].replaces[j].angle;
			}
		}
	}
	return root;
}

void GameState::saveWorld(const std::string& filePath)
{
	Json::Value gameValue{getJsonValueFromGame()};

	//Load all generic entities
	Json::Value genericEntities;
	for(auto& directory_entry : boost::filesystem::directory_iterator(Context::getParameters().resourcesPath + "levels/entities"))
		JsonHelper::merge(genericEntities, JsonHelper::loadFromFile(directory_entry.path().generic_string()));

	for(const std::string& entityName : gameValue["entities"].getMemberNames())
	{
		Json::Value& entityValue{gameValue["entities"][entityName]};
		for(const std::string& genericEntityName : genericEntities["entities"].getMemberNames())
		{
			const Json::Value& genericEntityValue{genericEntities["entities"][genericEntityName]};
			if(JsonHelper::isSubset(genericEntityValue, entityValue))
			{
				JsonHelper::substract(entityValue, genericEntityValue);
				entityValue["base"] = genericEntityName;
				break;
			}
		}
	}

	JsonHelper::saveToFile(gameValue, filePath);
}

void GameState::initWorld(const std::string& filePath)
{
	Context::getEventManager().emit<LoadingStateChange>("Loading save file");
	try
	{
		Json::Value root{JsonHelper::loadFromFile(filePath)};
		const Json::Value model{JsonHelper::loadFromFile(Context::getParameters().resourcesPath + "levels/model.json")};

		//Parse the save file from the model file
		JsonHelper::parse(root, model);

		if(root.isMember("level"))
		{
			Json::Value levelToLoad{JsonHelper::loadFromFile(Context::getParameters().resourcesPath + "levels/" + root["level"].asString())};
			//Parsing of the included file from the model file
			JsonHelper::parse(levelToLoad, model);
			JsonHelper::merge(root, levelToLoad);
		}

		loadSaveInformations(root);

		Json::Value genericEntities;
		if(root.isMember("import"))
			genericEntities = loadGenericEntities(root["import"], model);

		if(root.isMember("entities"))
		{
			Context::getEventManager().emit<LoadingStateChange>("Loading entities");
			deserializeEntities(root["entities"], genericEntities);
		}

		if(not Context::getParameters().debugMode)
		{
			Context::getEventManager().emit<LoadingStateChange>("Loading background");
			loadReplaces(root["level"]["replaces"]);
			loadBackground();
		}

		addSkyAnimations();
		getSystem<ScrollingSystem>().searchPlayer();
		Context::getSystemManager().update<ScrollingSystem>(0.f);
		Context::getSystemManager().update<AnimationsSystem>(0.f);
		Context::getPlayer().handleInitialInputState();
		requestStackPop();
		Context::getEventManager().emit<LoadingStateChange>("Loading HUD");
		requestStackPush<HUDState>();
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		std::cerr << "Failed to load save file \"" << filePath << "\".\n";
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
			Context::getWorld().DestroyBody(entity.second.component<BodyComponent>()->body);

		entity.second.destroy();
	}
	for(auto& sceneEntity : m_sceneEntities)
		sceneEntity.second.destroy();
}

void GameState::loadSaveInformations(const Json::Value& root)
{
	const Json::Value time{root["time"]};
	getSystem<TimeSystem>().setTotalTime(sf::seconds(time["date"].asFloat()));
	m_timeSpeed = time["speed"].asFloat();
	getSystem<AnimationsSystem>().setTimeSpeed(m_timeSpeed);

	const Json::Value level{root["level data"]};
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
	m_levelRect.width = levelBox["w"].asFloat();
	m_levelRect.height = levelBox["h"].asFloat();
	m_levelRect.top = levelBox["x"].asFloat();
	m_levelRect.left = levelBox["y"].asFloat();

	getSystem<ScrollingSystem>().setLevelData(m_levelRect, m_referencePlan);
}

Json::Value GameState::loadGenericEntities(const Json::Value& includes, const Json::Value& model)
{
	Json::Value genericEntities;
	for(Json::ArrayIndex i{0}; i < includes.size(); ++i)
	{
		//Parse the imported data
		Json::Value includeRoot{JsonHelper::loadFromFile(Context::getParameters().resourcesPath + "levels/entities/" + includes[i].asString())};

		//Parsing of the included file from the model file
		JsonHelper::parse(includeRoot, model);

		//Add generic entities to others
		Json::Value currentGenericEntities{includeRoot["entities"]};
		for(const std::string& genericEntityName : currentGenericEntities.getMemberNames())
			genericEntities[genericEntityName] = currentGenericEntities[genericEntityName];
	}
	return genericEntities;
}

void GameState::deserializeEntities(Json::Value& jsonEntities, const Json::Value& genericEntities)
{
	Serializer s(m_entities, jsonEntities);
	std::set<std::string> alreadyDeserialized;
	std::function<void(const std::string&)> deserializerLambda = [&](const std::string& entityName)
	{
		if(alreadyDeserialized.count(entityName) > 0)
			return;

		entityx::Entity& entity{m_entities.emplace(entityName, Context::getEntityManager().create()).first->second};
		Json::Value& jsonEntity{jsonEntities[entityName]};

		//If the entity is derivated from a base entities, add json values of the base to the json entity
		if(jsonEntity.isMember("base"))
		{
			const std::string baseName{jsonEntity["base"].asString()};
			if(not TEST(genericEntities.isMember(baseName)))
				throw std::runtime_error("\"" + baseName + "\" is not a valid base.");
			JsonHelper::merge(jsonEntity, genericEntities[baseName]);
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

void GameState::loadReplaces(const Json::Value& replaces)
{
	//for each group of replaces
	for(const std::string groupOfReplacesName : replaces.getMemberNames())
	{
		//Filename of the image to load
		const std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
		//Path of the image to load
		const std::string path{Context::getParameters().resourcesPath + "images/levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};

		const Json::Value groupOfReplaces{replaces[groupOfReplacesName]};
		//Vector that will be added to m_sceneEntitiesData
		std::vector<SceneReplaces> planData;
		//For each image frame in the group of replaces
		for(Json::ArrayIndex i{0}; i < groupOfReplaces.size(); i++)
		{
			const Json::Value image{groupOfReplaces[i]};
			const Json::Value origin{image["origin"]};

			//Coordinates of the original image
			sf::IntRect originRect;
			originRect.left = origin["x"].asInt();
			originRect.top = origin["y"].asInt();
			originRect.width = origin["w"].asInt();
			originRect.height = origin["h"].asInt();

			SceneReplaces replacesData;
			replacesData.origin = {origin["x"].asInt(), origin["y"].asInt(), origin["w"].asInt(), origin["h"].asInt()};

			//Load the texture
			//Identifier of the texture, in format "level_plan_texture"
			const std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
			Context::getTextureManager().load(textureIdentifier, path, originRect);
			//Replaces
			const Json::Value currentReplaces{image["replaces"]};
			//For each replacing of the image
			for(Json::ArrayIndex j{0}; j < currentReplaces.size(); j++)
			{
				const Json::Value replace{currentReplaces[j]};
				//Identifier of the entity, in format "level_plan_texture_replace"
				const std::string replaceIdentifier{textureIdentifier + "_" + std::to_string(j)};
				//Position where the frame should be replaced
				Transform replaceTransform;
				replaceTransform.x = replace["x"].asFloat();
				replaceTransform.y = replace["y"].asFloat();
				replaceTransform.z = replace["z"].asFloat();
				replaceTransform.angle = replace["angle"].asFloat();

				replacesData.replaces.push_back({replace["x"].asFloat(), replace["y"].asFloat(), replace["z"].asFloat(), replace["angle"].asFloat()});

				//Create an entity
				m_sceneEntities.emplace(replaceIdentifier, Context::getEntityManager().create());
				//Create a sprite with the loaded texture
				//Assign the sprite to the entity
				sf::Sprite replaceSpr(Context::getTextureManager().get(textureIdentifier));
				replaceSpr.setPosition(replaceTransform.x, replaceTransform.y);
				replaceSpr.setRotation(replaceTransform.angle);
				SpriteComponent::Handle sprComp{m_sceneEntities[replaceIdentifier].assign<SpriteComponent>()};
				sprComp->sprite = replaceSpr;
				TransformComponent::Handle trsfComp{m_sceneEntities[replaceIdentifier].assign<TransformComponent>()};
				trsfComp->transform = replaceTransform;
				CategoryComponent::Handle catComp{m_sceneEntities[replaceIdentifier].assign<CategoryComponent>()};
				catComp->category = {Category::Scene};
			}
			planData.push_back(replacesData);
		}
		m_sceneEntitiesData.emplace(groupOfReplacesName, planData);
	}
}

void GameState::loadBackground()
{
	const float chunkSize{static_cast<float>(sf::Texture::getMaximumSize())};
	const float viewWidth{Context::getParameters().defaultViewSize.x};
	const float scalableLevelWidth{m_levelRect.width - viewWidth};
	for(unsigned int i{0}; i < m_numberOfPlans; ++i)
	{
		const std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
		const std::string path{Context::getParameters().resourcesPath + "images/levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
		//The length of the plan, relatively to the reference
		const float planLength{scalableLevelWidth * std::pow(1.5f, m_referencePlan - static_cast<float>(i)) + viewWidth};
		//Number of chunks to load in this plan
		const int numberOfChunks{static_cast<int>(planLength / chunkSize) + 1};

		for(int j{0}; j < numberOfChunks; ++j)
		{
			//Identifier of the entity, in format "level_plan_chunk"
			const std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
			//Size of the chunk to load, may be truncated if we reach the end of the image.
			float currentChunkSize{chunkSize};
			if(j == numberOfChunks - 1)
				currentChunkSize = planLength - chunkSize * static_cast<float>(j);
			Context::getTextureManager().load<sf::IntRect>(textureIdentifier, path,
					sf::IntRect(j * static_cast<int>(chunkSize), 0, static_cast<int>(currentChunkSize), static_cast<int>(m_levelRect.height)));
			//Create an entity
			m_sceneEntities.emplace(textureIdentifier, Context::getEntityManager().create());
			//Create a sprite with the loaded texture
			//Assign the sprite to the entity
			sf::Sprite chunkSpr(Context::getTextureManager().get(textureIdentifier));
			chunkSpr.setPosition(static_cast<float>(j*chunkSize), 0.f);
			SpriteComponent::Handle sprComp{m_sceneEntities[textureIdentifier].assign<SpriteComponent>()};
			sprComp->sprite = chunkSpr;
			TransformComponent::Handle trsfComp{m_sceneEntities[textureIdentifier].assign<TransformComponent>()};
			trsfComp->transform = {static_cast<float>(j) * chunkSize, 0.f, static_cast<float>(i), 0.f};
			CategoryComponent::Handle catComp{m_sceneEntities[textureIdentifier].assign<CategoryComponent>()};
			catComp->category = {Category::Scene};
			//Update the ScrollingSystem in order to directly display the sprite at the right position
		}
	}
}

void GameState::addSkyAnimations()
{
	const float daySeconds{std::remainder(getSystem<TimeSystem>().getRealTime().asSeconds(), 600.f)};
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
}

template <class SystemType>
inline SystemType& GameState::getSystem()
{
	return *Context::getSystemManager().system<SystemType>();
}
