#ifndef KEYCONFIGURATIONSTATE_H
#define KEYCONFIGURATIONSTATE_H

#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
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
class KeyConfigurationState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
		KeyConfigurationState(StateStack& stack);

		/// Default destructor.
		~KeyConfigurationState();

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
		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::Panel::Ptr m_background;           ///< The background of the menu.
		tgui::Label::Ptr m_title;                ///< The title label of the menu
};

#endif//KEYCONFIGURATIONSTATE_H

