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