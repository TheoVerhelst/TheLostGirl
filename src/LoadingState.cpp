#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <TheLostGirl/State.h>

#include <TheLostGirl/LoadingState.h>

LoadingState::LoadingState(StateStack& stack, Context context) :
	State(stack, context)
{
}

void LoadingState::draw()
{
}

bool LoadingState::update(sf::Time)
{
	return true;
}

bool LoadingState::handleEvent(const sf::Event&)
{
	return false;
}