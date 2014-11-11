#ifndef STATESTACK_H
#define STATESTACK_H

#include <vector>
#include <utility>
#include <functional>
#include <map>
#include <cassert>

#include <SFML/System/NonCopyable.hpp>

#include <TheLostGirl/StateIdentifiers.h>
#include <TheLostGirl/State.h>

namespace sf
{
	class Event;
	class Time;
}

/// States manager.
/// This class holds all the game or menu state and manages them.
/// \see State
class StateStack : private sf::NonCopyable
{
	public:
		/// Various actions to do on states.
		enum Action
		{
			Push,  ///< Push a new state on the stack.
			Pop,   ///< Remove the last state from the stack.
			Clear, ///< Remove all the states from the stack.
		};
		
		/// Default constructor.
		/// \param context Current context of the application.
		/// \see State::Context
		explicit StateStack(State::Context context);
		
		/// Register a new state.
		/// Try to add a state that was not already registred makes undefined behavior.
		/// \param stateID Identifier of the state to register.
		template <typename T>
		void registerState(States stateID);
		
		/// Call the update() function of all the states in the stack.
		/// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);
		
		/// Call the draw() function of all the states in the stack.
		void draw();
		
		/// Call the handleEvent() function of all the states in the stack.
		/// \param event SFML event to handle.
		void handleEvent(const sf::Event& event);
		
		/// Add a new state on the pending list, and construct it as soon as possible.
		/// \param stateID Identifier of the state to add.
		void pushState(States stateID);
		
		/// Delete the top state as soon as possible.
		void popState();
		
		/// Delete all the states as soon as possible.
		void clearStates();
		
		/// Check if the stack is empty or not.
		/// \return True if the stack is empty, false otherwise.
		bool isEmpty() const;
		
		/// Acces the current context.
		/// \return The current context.
		State::Context& getContext();

	private:
		/// Construct the given state.
		/// \param stateID Identifier of the state to construct.
		/// \return A pointer to the created state.
		State::Ptr createState(States stateID);
		
		/// Remove states that need to be removed, constructs others states, ...
		void applyPendingChanges();
		
		/// Struct that represents change that need to be done in the stack.
		struct PendingChange
		{
			Action action;  ///< Action to do.
			States stateID; ///< Identifier of the state on wich to do the action.
			
			/// Default constructor.
			/// \param _action Action to do.
			/// \param _stateID Identifier of the state on wich to do the action.
			explicit PendingChange(Action _action, States _stateID = States::None);
		};
		
		std::vector<State::Ptr> m_stack;                           ///< Array of pointers to the states.
		std::vector<PendingChange> m_pendingList;                  ///< List of all changes to do in the next update.
		State::Context m_context;                                  ///< Context of the application.
		std::map<States, std::function<State::Ptr()> > m_factories;///< Array of constructors.
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