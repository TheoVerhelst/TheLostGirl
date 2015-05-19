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
#include <TheLostGirl/states/MainMenuState.h>
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
#include <TheLostGirl/serialization.h>

#include <TheLostGirl/states/EmptyLevelState.h>

EmptyLevelState::EmptyLevelState(StateStack& stack) :
	State(stack),
	m_sceneEntities(),
	m_timeSpeed{1.f},
	m_threadLoad(),
	m_levelIdentifier{""},
	m_numberOfPlans{1},
	m_referencePlan{0.f},
	m_levelRect{0, 0, 1920, 1080}
{
	m_threadLoad = std::thread([this](const std::string& str){return this->initWorld(str);}, "resources/levels/save.json");
}

EmptyLevelState::~EmptyLevelState()
{
	if(m_threadLoad.joinable())
		m_threadLoad.join();
	for(auto& sceneEntity : m_sceneEntities)
		sceneEntity.second.destroy();
}

void EmptyLevelState::draw()
{
	getContext().systemManager.update<RenderSystem>(0.f);
}

bool EmptyLevelState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<AnimationsSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<PendingChangesSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<ScrollingSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<TimeSystem>(elapsedTime.asSeconds()*m_timeSpeed);//Scale the time
	return false;
}

bool EmptyLevelState::handleEvent(const sf::Event&)
{
	return false;
}

void EmptyLevelState::initWorld(const std::string& file)
{
	const float scale{getContext().parameters.scale};
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
			throw std::runtime_error("\"" + file + "\": " + reader.getFormattedErrorMessages());
		if(!reader.parse(modelSaveFile, model))
			throw std::runtime_error("\"resources/levels/model.json\": " + reader.getFormattedErrorMessages());

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

		//level
		//for each group of replaces
		for(std::string groupOfReplacesName : level["replaces"].getMemberNames())
		{
			//Filename of the image to load
			std::string fileTexture{m_levelIdentifier + "_" + groupOfReplacesName};
			//Path of the image to load
			std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};

			const Json::Value groupOfReplaces{level["replaces"][groupOfReplacesName]};
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

				//Load the texture
				//Identifier of the texture, in format "level_plan_texture"
				std::string textureIdentifier{fileTexture + "_" + std::to_string(i)};
				getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading plan") + L" " + std::wstring(groupOfReplacesName.begin(), groupOfReplacesName.end()));
				texManager.load<sf::IntRect>(textureIdentifier, path, originRect);
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

					//Create an entity
					m_sceneEntities.emplace(replaceIdentifier, getContext().entityManager.create());
					//Create a sprite with the loaded texture
					//Assign the sprite to the entity
					sf::Sprite replaceSpr(texManager.get(textureIdentifier));
					replaceSpr.setPosition(replaceTransform.x, replaceTransform.y);
					replaceSpr.setRotation(replaceTransform.angle);
					std::unordered_map<std::string, sf::Sprite> sprites{{"main", replaceSpr}};
					std::unordered_map<std::string, Transform> transforms{{"main", replaceTransform}};
					SpriteComponent::Handle sprComp{m_sceneEntities[replaceIdentifier].assign<SpriteComponent>()};
					sprComp->sprites = sprites;
					TransformComponent::Handle trsfComp{m_sceneEntities[replaceIdentifier].assign<TransformComponent>()};
					trsfComp->transforms = transforms;
					CategoryComponent::Handle catComp{m_sceneEntities[replaceIdentifier].assign<CategoryComponent>()};
					catComp->category = Category::Scene;
					//Update the ScrollingSystem in order to directly display the sprite at the right position
					getContext().systemManager.update<ScrollingSystem>(0.f);
				}
			}
		}
		//Load all the image in multiples chunks
		for(unsigned int i{0}; i < m_numberOfPlans; ++i)
		{
			std::string fileTexture{m_levelIdentifier + "_" + std::to_string(i)};
			std::string path{paths[getContext().parameters.scaleIndex] + "levels/" + m_levelIdentifier + "/" + fileTexture + ".png"};
			int chunkSize{int(sf::Texture::getMaximumSize())};
			//The length of the plan, relatively to the reference.
			int planLength{int(float(m_levelRect.width) * std::pow(1.5f, m_referencePlan - float(i)) * getContext().parameters.scale)};
			//Number of chunks to load in this plan
			int numberOfChunks{(planLength/chunkSize)+1};

			for(int j{0}; j < numberOfChunks; ++j)
			{
				//Identifier of the entity, in format "level_plan_chunk"
				std::string textureIdentifier{fileTexture + "_" + std::to_string(j)};
				//Size of the chunk to load, may be truncated if we reach the end of the image.
				int currentChunkSize{chunkSize};
				if(j >= planLength/chunkSize)
					currentChunkSize = planLength - chunkSize*j;
				getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading plan") + L" " + std::to_wstring(i+1));
				texManager.load<sf::IntRect>(textureIdentifier, path, sf::IntRect(j*chunkSize, 0, currentChunkSize, int(float(m_levelRect.height)*getContext().parameters.scale)));
				//Create an entity
				m_sceneEntities.emplace(textureIdentifier, getContext().entityManager.create());
				//Create a sprite with the loaded texture
				//Assign the sprite to the entity
				sf::Sprite chunkSpr(texManager.get(textureIdentifier));
				chunkSpr.setPosition(float(j*chunkSize), 0);
				std::unordered_map<std::string, sf::Sprite> sprites{{"main", chunkSpr}};
				std::unordered_map<std::string, Transform> transforms{{"main", {float(j*chunkSize), 0, float(i), 0}}};
				SpriteComponent::Handle sprComp{m_sceneEntities[textureIdentifier].assign<SpriteComponent>()};
				sprComp->sprites = sprites;
				TransformComponent::Handle trsfComp{m_sceneEntities[textureIdentifier].assign<TransformComponent>()};
				trsfComp->transforms = transforms;
				CategoryComponent::Handle catComp{m_sceneEntities[textureIdentifier].assign<CategoryComponent>()};
				catComp->category = Category::Scene;
				//Update the ScrollingSystem in order to directly display the sprite at the right position
				getContext().systemManager.update<ScrollingSystem>(0.f);
			}
		}

		//Load the day sky
		const std::string dayIdentifier{"day sky"};
		sf::Vector2f position{1920.f/2.f, 1080.f};
		sf::Vector2f origin{2900.f/2.f, 2900.f/2.f};
		getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading day sky"));
		texManager.load(dayIdentifier, paths[getContext().parameters.scaleIndex] + "day.png");
		//Create a sprite with the loaded texture
		sf::Sprite daySpr(texManager.get(dayIdentifier));
		//Assign origin of the sprite to the center of the day image
		daySpr.setOrigin(origin*scale);

		//Load the night sky
		const std::string nightIdentifier{"night sky"};
		getContext().eventManager.emit<LoadingStateChange>(LangManager::tr("Loading night sky"));
		texManager.load(nightIdentifier, paths[getContext().parameters.scaleIndex] + "night.png");
		//Create a sprite with the loaded texture
		sf::Sprite nightSpr(texManager.get(nightIdentifier));
		//Assign origin of the sprite to the center of the night image
		nightSpr.setOrigin(origin*scale);

		//Create an entity
		m_sceneEntities.emplace("sky", getContext().entityManager.create());
		//Assign the sprites to the entity, and set its z-ordinates to positive infinity
		std::unordered_map<std::string, sf::Sprite> skySprites{{"day", daySpr}, {"night", nightSpr}};
		std::unordered_map<std::string, Transform> skyTransforms{{"day", {position.x, position.y, std::numeric_limits<float>::infinity(), 0}},
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
		float daySeconds{std::remainder(getContext().systemManager.system<TimeSystem>()->getRealTime().asSeconds(), 600.f)};
		skyAnimationsComp->animationsManagers["main"].setProgress("day/night cycle", daySeconds/600.f);
		skyAnimationsComp->animationsManagers["main"].play("day/night cycle");
	}
	catch(std::runtime_error& e)
	{
		std::cerr << e.what() << "\n";
		std::cerr << "Failed to load save file \"" << file << "\".\n";
		//Clear game content in order to prevent segmentation faults.
		for(auto& sceneEntity : m_sceneEntities)
			sceneEntity.second.destroy();
	}
	//Pop the intro menu and push the main menu
	requestStackPop();
	requestStackPush<MainMenuState>();
}
