#include <SFML/Window/Event.hpp>

#include <TheLostGirl/states/ParametersState.h>

ParametersState::ParametersState(StateStack& stack) :
	State(stack)
{
}

ParametersState::~ParametersState()
{
}

void ParametersState::draw()
{
}

bool ParametersState::update(sf::Time)
{
	return false;
}

bool ParametersState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPop();
	return false;
}

