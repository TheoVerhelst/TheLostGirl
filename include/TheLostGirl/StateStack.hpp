#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include <vector>
#include <memory>
#include <SFML/System.hpp>

//Forward declarations
namespace sf
{
	class Time;
	class Event;
}

class State;

/// States manager.
/// This class holds all the game or menu state and manages them.
/// \see State
class StateStack : private sf::NonCopyable
{
	public:
		/// Destructor;
		~StateStack() = default;

		/// Call the update() function of all the states in the stack.
		/// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);

		/// Call the draw() function of all the states in the stack.
		void draw();

		/// Call the handleEvent() function of all the states in the stack.
		/// \param event SFML event to handle.
		void handleEvent(const sf::Event& event);

		/// Add a new state of the templated type on the pending list, and construct it as soon as possible.
		/// \param args Arguments that will be forwarded to the constructor of the state.
		/// There can be zero, one or more arguments of any types.
		template <typename T, typename ... Args>
		void pushState(Args&& ... args);

		/// Delete the top state as soon as possible.
		void popState();

		/// Delete all the states as soon as possible.
		void clearStates();

		/// Check if the stack is empty or not.
		/// \return True if the stack is empty, false otherwise.
		bool isEmpty() const;

	private:
		/// Remove states that need to be removed, constructs others states, ...
		void applyPendingChanges();

		std::vector<std::unique_ptr<State>> m_stack;     ///< Array of pointers to the states.
		std::vector<std::function<void()>> m_pendingList;///< List of all changes to do in the next update.
};

template <typename T, typename ... Args>
void StateStack::pushState(Args&& ... args)
{
	m_pendingList.push_back([this, args...]
	{
		//No perfect forwarding is done here because the arguments
		//need to be copied in order to be stored in the lambda
		m_stack.push_back(std::unique_ptr<State>(new T(args...)));
	});
}

#endif//STATESTACK_HPP
