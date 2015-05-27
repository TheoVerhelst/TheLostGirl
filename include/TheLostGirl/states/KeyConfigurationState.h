#ifndef KEYCONFIGURATIONSTATE_H
#define KEYCONFIGURATIONSTATE_H

#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/Button.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TheLostGirl/State.h>
#include <TheLostGirl/Player.h>

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

		/// Scroll the area with labels and buttons.
		void scrollArea(int newScrollValue);

		tgui::Panel::Ptr m_background;                                ///< The background of the menu.
		tgui::Label::Ptr m_title;                                     ///< The title label of the menu.
		tgui::Label::Ptr m_nameLabel;                                 ///< The label of the name column.
		tgui::Label::Ptr m_key1Label;                                 ///< The title of the first keys binding column.
		tgui::Label::Ptr m_key2Label;                                 ///< The title of the second keys binding column.
		tgui::Label::Ptr m_mouseLabel;                                ///< The title of the mouse buttons binding column.
		tgui::Label::Ptr m_joystickLabel;                             ///< The title of the joystick buttons binding column.
		tgui::Panel::Ptr m_actionsPanel;                              ///< Scrollable panel for displaying the actions widgets.
		tgui::Scrollbar::Ptr m_scrollbar;                             ///< Scrollbar of m_actionsPanel.
		std::map<Player::Action, sf::String> m_actionStrings;         ///< The strings corresponding to actions (just a translation map).
		std::map<Player::Action, tgui::Label::Ptr> m_actionLabels;    ///< The gui labels corresponding to actions.
		std::map<Player::Action, tgui::Button::Ptr> m_key1Buttons;    ///< The gui buttons corresponding to actions binded with keys (first binding).
		std::map<Player::Action, tgui::Button::Ptr> m_key2Buttons;    ///< The gui buttons corresponding to actions binded with keys (second binding).
		std::map<Player::Action, tgui::Button::Ptr> m_mouseButtons;   ///< The gui buttons corresponding to actions binded with mouse buttons.
		std::map<Player::Action, tgui::Button::Ptr> m_joystickButtons;///< The gui buttons corresponding to actions binded with joystick buttons.
		tgui::Button::Ptr m_cancelButton;                             ///< The cancel button.
		tgui::Button::Ptr m_okButton;                                 ///< The ok button.
};

#endif//KEYCONFIGURATIONSTATE_H

