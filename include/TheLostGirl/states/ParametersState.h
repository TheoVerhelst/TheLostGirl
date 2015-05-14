#ifndef PARAMETERSSTATE_H
#define PARAMETERSSTATE_H

#include <TheLostGirl/State.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}

class StateStack;

/// State that effectively play the game.
/// The game become really interesting here.
class ParametersState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
		ParametersState(StateStack& stack);

		/// Default destructor.
		~ParametersState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// This function call e.g. the physic update function, the AI function, etc...
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);

	private:
};

#endif//PARAMETERSSTATE_H
