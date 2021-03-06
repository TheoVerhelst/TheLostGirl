#ifndef MAINMENUSTATE_HPP
#define MAINMENUSTATE_HPP

#include <memory>
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

/// Cannot be more explicit.
class MainMenuState : public State
{
	public:
		/// Default constructor.
		MainMenuState();

		/// Destructor.
		/// It clear the GUI.
		~MainMenuState();

		/// The drawing function.
		/// \return virtual void
		/// It must do all things related to drawing stuff on the screen.
		virtual void draw() override;

		/// The logic update function.
		/// \param dt Elapsed time in the last game frame.
		/// \return Return true if the state under this one in the stack must be also updated.
		/// This function call e.g. the physic update function, the AI function, etc...
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
		/// Context type of this class.
		typedef ContextAccessor<
				ContextElement::EventManager,
				ContextElement::Parameters,
				ContextElement::LangManager,
				ContextElement::Gui> Context;

		/// Play the game (usually on press of the play button).
		void playGame();

		/// Exit the game (usually on press of the exit button).
		void exitGame();

		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::Panel::Ptr m_background;            ///< The background of the menu.
		tgui::Picture::Ptr m_logo;                ///< The Lost Girl logo.
		tgui::VerticalLayout::Ptr m_buttonsLayout;///< The layout for the buttons.
		tgui::Button::Ptr m_newButton;            ///< The New game button.
		tgui::Button::Ptr m_loadButton;           ///< The Load game button.
		tgui::Button::Ptr m_exitButton;           ///< The Exit button.
};

#endif//MAINMENUSTATE_HPP
