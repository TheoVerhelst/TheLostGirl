#ifndef STATESTACK_H
#define STATESTACK_H

#include <vector>
#include <utility>
#include <functional>
#include <map>
#include <cassert>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <TGUI/TGUI.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/StateIdentifiers.h>
#include <TheLostGirl/ResourceIdentifiers.h>


namespace sf
{
	class Event;
	class RenderWindow;
}

class StateStack : private sf::NonCopyable
{
	public:
		enum Action
		{
			Push,
			Pop,
			Clear,
		};
		explicit StateStack(State::Context context);
		template <typename T>
		void registerState(States stateID);
		void update(sf::Time dt);
		void draw();
		void handleEvent(const sf::Event& event);
		void pushState(States stateID);
		void popState();
		void clearStates();
		bool isEmpty() const;

	private:
		State::Ptr createState(States stateID);
		void applyPendingChanges();
		struct PendingChange
		{
			explicit PendingChange(Action _action, States _stateID = States::None);
			Action action;
			States stateID;
		};
		std::vector<State::Ptr> m_stack;
		std::vector<PendingChange> m_pendingList;
		State::Context m_context;
		std::map<States, std::function<State::Ptr()> > m_factories;
};

template <typename T>
void StateStack::registerState(States stateID)
{
	m_factories[stateID] = [this]()
	{
		return State::Ptr(new T(*this, m_context));
	};
}

#endif // STATESTACK_H