#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_context(context),
	m_player(),
	m_commandQueue(),
	m_archer(m_context.entityManager.create())
{
	sf::Texture& archerTexture = m_context.textures.get(Textures::Archer);
	sf::Sprite archerSprite(archerTexture);
	m_archer.assign<SpriteComponent>(archerSprite);
}

void GameState::draw()
{
}

bool GameState::update(sf::Time)
{
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
	return true;
}