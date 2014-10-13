#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/Player.h>

#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_context(context),
	m_archer(m_context.entityManager.create())
{
	sf::Texture& archerTexture = m_context.textureManager.get(Textures::Archer);
	sf::Sprite* archerSprite = new sf::Sprite(archerTexture);
	m_archer.assign<SpriteComponent>(archerSprite);
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	switch(event.type)
	{
		case sf::Event::Closed:
			m_archer.destroy();
			requestStackPop();
			break;

		case sf::Event::KeyPressed:
			if(event.key.code == sf::Keyboard::Escape)
			{
				m_archer.destroy();
				requestStackPop();
				requestStackPush(States::MainMenu);
			}
			break;

		default:
			break;
	}
	getContext().player.handleEvent(event, getContext().commandQueue);
	return false;
}