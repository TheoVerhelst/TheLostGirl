#include <cassert>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <TheLostGirl/State.h>

#include <TheLostGirl/StateStack.h>

StateStack::StateStack(Context context) :
	m_stack(),
	m_pendingList(),
	m_context(context)
{
}

StateStack::~StateStack()
{
}

void StateStack::update(sf::Time dt)
{
	// Iterate from top to bottom, stop as soon as update() returns false
	for(auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
		if(!(*itr)->update(dt))
			break;
	applyPendingChanges();
}

void StateStack::draw()
{
	// Draw all active states from bottom to top
	for(State::Ptr& state : m_stack)
		state->draw();
}

void StateStack::handleEvent(const sf::Event& event)
{
	// Iterate from top to bottom, stop as soon as handleEvent() returns false
	for(auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
	{
		if(!(*itr)->handleEvent(event))
			break;
	}
	applyPendingChanges();
}

void StateStack::popState()
{
	m_pendingList.push_back([this]{m_stack.pop_back();});
}

void StateStack::clearStates()
{
	m_pendingList.push_back([this]{m_stack.clear();});
}

bool StateStack::isEmpty() const
{
	return m_stack.empty();
}

StateStack::Context StateStack::getContext()
{
	return m_context;
}

void StateStack::applyPendingChanges()
{
	for(auto& change : m_pendingList)
		change();

	m_pendingList.clear();
}

StateStack::Context::Context(Parameters& _parameters,
						sf::RenderWindow& _window,
						TextureManager& _textureManager,
						FontManager& _fontManager,
						ScriptManager& _scriptManager,
						tgui::Gui& _gui,
						entityx::EventManager& _eventManager,
						entityx::EntityManager& _entityManager,
						entityx::SystemManager& _systemManager,
						b2World& _world,
						Player& _player
						):
	parameters(_parameters),
	window(_window),
	textureManager(_textureManager),
	fontManager(_fontManager),
	scriptManager(_scriptManager),
	gui(_gui),
	eventManager(_eventManager),
	entityManager(_entityManager),
	systemManager(_systemManager),
	world(_world),
	player(_player)
{
}
