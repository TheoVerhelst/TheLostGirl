#include <SFML/Window/Event.hpp>
#include <TheLostGirl/states/KeyConfigurationState.h>

KeyConfigurationState::KeyConfigurationState(StateStack& stack) :
	State(stack)
{
}

KeyConfigurationState::~KeyConfigurationState()
{
}

void KeyConfigurationState::draw()
{
}

bool KeyConfigurationState::update(sf::Time)
{
	return false;
}

bool KeyConfigurationState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPop();
	return false;
}

