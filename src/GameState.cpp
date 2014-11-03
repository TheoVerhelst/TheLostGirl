#include <iostream>

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
#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_sprites(),
	m_animations(),
	m_fallingListener(),
	m_timeSystem()
{
	Json::Value root; // will contains the root value after parsing.
	Json::Reader reader;
	std::ifstream saveFileStream("ressources/levels/save.json", std::ifstream::binary);
	bool parsingSuccessful = reader.parse(saveFileStream, root);
	if(!parsingSuccessful)
	{
		//report to the user the failure and their locations in the document.
		std::cerr << "Failed to parse save file " << "ressources/levels/save.json" << ":" << std::endl
		<< reader.getFormattedErrorMessages() << std::endl;
	}
	initWorld(root);
	getContext().player.handleInitialInputState(getContext().commandQueue);
}

GameState::~GameState()
{
	for(auto& pair : m_entities)
	{
		getContext().world.DestroyBody(pair.second.component<Body>()->body);
		pair.second.destroy();
	}
	getContext().world.ClearForces();
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
	//The drag and drop system draw a line on the screen, so we must put it here
	getContext().systemManager.update<DragAndDropSystem>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<FallSystem>(elapsedTime.asSeconds());
	m_timeSystem.update(elapsedTime);
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

void GameState::initWorld(const Json::Value& levelData)
{
//	float scale = getContext().parameters.scale;
//	float pixelScale = getContext().parameters.pixelScale;
//	sf::Vector2f screenSize = getContext().parameters.worldFrameSize;
	
	//Parse the level data
	if(not levelData.type() == Json::objectValue)
		throw std::runtime_error("Failed to parse save file : not object value as root value.");
	else
	{
		if(levelData.isMember("entities"))
		{
			Json::Value entities = levelData["entities"];
			if(not entities.type() == Json::objectValue)
				throw std::runtime_error("Failed to parse save file : entities value not an object value.");
			else
			{
				Json::Value::Members entitiesNames = entities.getMemberNames();
				for(std::string& entityName : entitiesNames)
				{
					Json::Value entity = entities[entityName];
					m_entities.emplace(entityName, getContext().entityManager.create());		
					std::cout << entityName << std::endl;
					if(entity.isMember("sprite"))
					{
						Json::Value sprite = entity["sprite"];
						if(not sprite.type() == Json::stringValue)
							throw std::runtime_error("Failed to parse save file : sprite value not an string value.");
						else
						{
							if(sprite.asString() == "archer")
								m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Archer)));
							else if(sprite.asString() == "arms")
								m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Arms)));
							else if(sprite.asString() == "bow")
								m_sprites.emplace(entityName, sf::Sprite(getContext().textureManager.get(Textures::Bow)));
							else
								throw std::runtime_error("Failed to parse save file : sprite name does not exists.");
							m_entities[entityName].assign<SpriteComponent>(&m_sprites[entityName]);
						}
					}
					if(entity.isMember("category"))
					{
						Json::Value category = entity["category"];
						if(not category.type() == Json::arrayValue)
							throw std::runtime_error("Failed to parse save file : category value not an array value.");
						else
						{
							unsigned int categories{0};
							for(Json::ArrayIndex i{0}; i < category.size(); ++i)
							{
								//For each cateory in the list, add it to the entity's category.
								if(category[i] == "player")
									categories |= Category::Player;
								else if(category[i] == "can fall")
									categories |= Category::CanFall;
								else if(category[i] == "ground")
									categories |= Category::Ground;
								else
									throw std::runtime_error("Failed to parse save file : category name does not exists.");
							}
							m_entities[entityName].assign<CategoryComponent>(categories);
						}
					}
					if(entity.isMember("walk"))
					{
						Json::Value walk = entity["walk"];
						if(not walk.type() == Json::realValue)
							throw std::runtime_error("Failed to parse save file : walk value not a real value.");
						else
							m_entities[entityName].assign<Walk>(walk.asFloat());
					}
					if(entity.isMember("jump"))
					{
						Json::Value jump = entity["jump"];
						if(not jump.type() == Json::realValue)
							throw std::runtime_error("Failed to parse save file : jump value not a real value.");
						else
							m_entities[entityName].assign<Jump>(jump.asFloat());
					}
					if(entity.isMember("direction"))
					{
						Json::Value direction = entity["direction"];
						if(not direction.type() == Json::stringValue)
							throw std::runtime_error("Failed to parse save file : direction value not an string value.");
						else
						{
							if(direction.asString() == "left")
								m_entities[entityName].assign<DirectionComponent>(Direction::Left);
							else if(direction.asString() == "right")
								m_entities[entityName].assign<DirectionComponent>(Direction::Right);
							else if(direction.asString() == "top")
								m_entities[entityName].assign<DirectionComponent>(Direction::Top);
							else if(direction.asString() == "bottom")
								m_entities[entityName].assign<DirectionComponent>(Direction::Bottom);
							else if(direction.asString() == "none")
								m_entities[entityName].assign<DirectionComponent>(Direction::None);
							else
								throw std::runtime_error("Failed to parse save file : direction name does not exists.");
						}
					}
					if(entity.isMember("fall"))
						m_entities[entityName].assign<FallComponent>();
				}
			}
		}
		if(levelData.isMember("joints"))
		{
			Json::Value joints = levelData["joints"];
			if(not joints.type() == Json::arrayValue)
				throw std::runtime_error("Failed to parse save file : entities value not an object value.");
			else
			{
			}
		}
	}
}