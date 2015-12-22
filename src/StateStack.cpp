#include <cassert>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>

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

void StateStack::applyPendingChanges()
{
	for(auto& change : m_pendingList)
		change();

	m_pendingList.clear();
}
