#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <TGUI/Picture.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/Label.hpp>

#include <TheLostGirl/State.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class Context;
class StateStack;

/// Introduction at the launching of the game.
class IntroState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
		IntroState(StateStack& stack);
		
		/// Default destructor.
		~IntroState();
		
        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);
		
	private:
		tgui::Panel::Ptr m_background; ///< The background of the menu.
		tgui::Picture::Ptr m_logo;     ///< The Lost Girl logo.
		tgui::Label::Ptr m_sentence;   ///< The sentence under the logo.
};

#endif//INTROSTATE_H
