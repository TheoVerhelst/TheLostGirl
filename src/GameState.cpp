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
	for(auto& entity : m_entities)
	{
		getContext().world.DestroyBody(entity.component<Body>()->body);
		entity.destroy();
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
	std::cout << levelData << std::endl;
	float scale = getContext().parameters.scale;
	float pixelScale = getContext().parameters.pixelScale;
	sf::Vector2f screenSize = getContext().parameters.worldFrameSize;
	
	//Parse the level data
}