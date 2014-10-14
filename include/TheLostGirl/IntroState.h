#ifndef INTROSTATE_H
#define INTROSTATE_H

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class State;
class Context;
class StateStack;

/// Introduction at the launching of the game.
class IntroState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack in where the State is added.
        /// \param context Context of the game.
		IntroState(StateStack& stack, Context context);
		
        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// \param dt Elapsed time since the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);
};

#endif // INTROSTATE_H
