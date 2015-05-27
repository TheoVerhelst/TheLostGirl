#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/State.h>

using entityx::EventManager;
using entityx::EntityManager;
using entityx::SystemManager;

State::State(StateStack& stack):
	m_stateStack(&stack)
{
}

State::~State()
{
}

void State::requestStackPop()
{
	m_stateStack->popState();
}

void State::requestStateClear()
{
	m_stateStack->clearStates();
}

StateStack::Context State::getContext() const
{
	return m_stateStack->getContext();
}
