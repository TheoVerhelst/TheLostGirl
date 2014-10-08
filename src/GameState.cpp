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
	
	//getContext().systemManager->add<Physics>(getContext().world);
	getContext().systemManager.add<Actions>(m_commandQueue);
	getContext().systemManager.add<Render>(getContext().window);
	getContext().systemManager.configure();//Init the manager
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	//getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
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
	m_player.handleEvent(event, m_commandQueue);
	return false;
}