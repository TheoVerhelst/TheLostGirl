#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <memory>

#include <TGUI/Button.hpp>
#include <TGUI/Picture.hpp>
#include <TGUI/Panel.hpp>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class Context;
class StateStack;

/// Cannot be more explicit.
class MainMenuState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param context Context of the game.
		MainMenuState(StateStack& stack, Context context);
		
		/// Default destructor.
		/// It clear the GUI.
		~MainMenuState();
		
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
		void playGame();
		tgui::Panel::Ptr m_background;
		tgui::Picture::Ptr m_logo;
		tgui::Button::Ptr m_newButton;
		tgui::Button::Ptr m_loadButton;
		tgui::Button::Ptr m_exitButton;
};

#endif // MAINMENUSTATE_H
