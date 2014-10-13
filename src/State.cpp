#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>

#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/StateStack.h>

#include <TheLostGirl/State.h>

using entityx::EventManager;
using entityx::EntityManager;
using entityx::SystemManager;

State::Context::Context(sf::RenderWindow& _window,
						TextureManager& _textureManager,
						FontManager& _fontManager,
						tgui::Gui& _gui,
						EventManager& _eventManager,
						EntityManager& _entityManager,
						SystemManager& _systemManager,
						b2World& _world
						):
	window(_window),
	textureManager(_textureManager),
	fontManager(_fontManager),
	gui(_gui),
	eventManager(_eventManager),
	entityManager(_entityManager),
	systemManager(_systemManager),
	world(_world)
{
}

State::State(StateStack& stack, Context context):
	m_stateStack(&stack),
	m_context(context)
{
}

State::~State()
{
}

void State::requestStackPush(States stateID)
{
	m_stateStack->pushState(stateID);
}

void State::requestStackPop()
{
	m_stateStack->popState();
}

void State::requestStateClear()
{
	m_stateStack->clearStates();
}

State::Context State::getContext() const
{
	return m_context;
}
