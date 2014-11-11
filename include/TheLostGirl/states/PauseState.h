#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include <TGUI/Button.hpp>
#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>

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
class PauseState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param context Current context of the application.
		PauseState(StateStack& stack, Context context);
		
		/// Default destructor.
		/// It clear the GUI.
		~PauseState();
		
        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);
		
	private:
		/// Return to the GameState state and delete this one.
		void backToGame();
		
		/// Add an OptionsState state in top of this one.
		void goToOptions();
		
		/// Go back to the main menu, delete this state and the GameState state.
		void backToMainMenu();
		
		tgui::Panel::Ptr m_background;           ///< The background of the menu.
		tgui::Label::Ptr m_pauseLabel;           ///< The pause label.
		tgui::Button::Ptr m_backToGameButton;    ///< The Back to game button.
		tgui::Button::Ptr m_goToOptionsButton;   ///< The Go to options button.
		tgui::Button::Ptr m_backToMainMenuButton;///< The Back to main menu button.
};

#endif // PAUSESTATE_H
