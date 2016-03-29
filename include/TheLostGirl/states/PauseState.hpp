#ifndef PAUSESTATE_HPP
#define PAUSESTATE_HPP

#include <TGUI/TGUI.hpp>
#include <TheLostGirl/State.hpp>
#include <TheLostGirl/Context.hpp>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class Context;
class StateStack;
struct ParametersChange;

/// Introduction at the launching of the game.
class PauseState : public State, private ContextAccessor<ContextElement::EventManager,
                                                         ContextElement::Parameters,
                                                         ContextElement::LangManager,
                                                         ContextElement::Gui>
{
	public:
        /// Constructor.
		PauseState();

		/// Destructor.
		/// It clear the GUI.
		~PauseState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() override;

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
		virtual bool update(sf::Time dt) override;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) override;

		/// Receive an event about a change in the parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

	private:
		/// Return to the GameState state and delete this one.
		inline void backToGame();

		/// Go back to the main menu, delete this state and the GameState state.
		inline void backToMainMenu();

		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::VerticalLayout::Ptr m_background;  ///< The background of the menu.
		tgui::Label::Ptr m_pauseLabel;           ///< The pause label.
		tgui::Button::Ptr m_backToGameButton;    ///< The Back to game button.
		tgui::Button::Ptr m_goToOptionsButton;   ///< The Go to options button.
		tgui::Button::Ptr m_backToMainMenuButton;///< The Back to main menu button.
};

#endif//PAUSESTATE_HPP
