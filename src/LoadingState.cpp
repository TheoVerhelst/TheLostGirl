#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <TheLostGirl/LoadingState.h>
#include <TheLostGirl/ResourceManager.h>

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

bool LoadingState::handleEvent(const sf::Event& event)
{
	return false;
}