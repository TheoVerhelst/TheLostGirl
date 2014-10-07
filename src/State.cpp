#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>

using entityx::EventManager;
using entityx::EntityManager;
using entityx::SystemManager;

State::Context::Context(sf::RenderWindow& _window, TextureManager& _textures, FontManager& _fonts, tgui::Gui& _gui, EventManager& _eventManager, EntityManager& _entityManager, SystemManager& _systemManager) :
	window(_window),
	textures(_textures),
	fonts(_fonts),
	gui(_gui),
	eventManager(_eventManager),
	entityManager(_entityManager),
	systemManager(_systemManager)
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
