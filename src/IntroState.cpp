#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>

#include <TheLostGirl/IntroState.h>

IntroState::IntroState(StateStack& stack, Context context) :
	State(stack, context)
{
}

void IntroState::draw()
{
}

bool IntroState::update(sf::Time)
{
	return true;
}

bool IntroState::handleEvent(const sf::Event& event)
{
	return true;
}