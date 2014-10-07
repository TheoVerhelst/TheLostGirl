#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_context(context),
	m_player(),
	m_commandQueue(),
	m_archer(getContext().entityManager.create())
{
	m_context.textures.load<sf::IntRect>(Textures::Archer, toPath(windowSize) + "charac.png", sf::IntRect(0, 0, 100, 200));
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
	return true;
}