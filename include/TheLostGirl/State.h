#ifndef STATE_H
#define STATE_H

#include <memory>
#include <entityx/Event.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Context.h>

namespace sf
{
	class Event;
	class Time;
}

/// Base class for the various game states.
/// In order to make a game state, you must inherit from this class and implement the pure virtual members.
/// The new state must then be registered and pushed in a StateStack.
/// The State class inherits from entityx::Receiver, allowing all states to receive events from entityx.
/// All states should at least implement a receiver function of the ParametersChange event,
/// because if the lang change then all texts must be updated in the state.
class State: public entityx::Receiver<State>
{
	public:
        /// Pointer typedef.
		typedef std::unique_ptr<State> Ptr;

		/// Destructor.
		virtual ~State() = default;

        /// The drawing function.
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() = 0;

        /// The logic update function.
        /// This function call e.g. the physic update function, the AI function, etc...
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
		virtual bool update(sf::Time dt) = 0;

        /// The event handling function.
        /// The state must decide what to do with the event.
        /// \note The closing window and resizing window events are already handled by the Application class.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
		virtual bool handleEvent(const sf::Event& event) = 0;

	protected:
		/// Add the given game state to the pending queue,
		/// and construct it as soon as possible.
		/// \param args Arguments to transfer to the state constructor.
		template<typename T, typename ... Args>
		void requestStackPush(Args&&... args);

		/// Delete the top state as soon as possible.
		void requestStackPop();

		/// Delete all the states as soon as possible.
		void requestStateClear();
};

template<typename T, typename ... Args>
void State::requestStackPush(Args&&... args)
{
	Context::stateStack->pushState<T>(std::forward<Args>(args)...);
}

#endif//STATE_H
