#ifndef PARAMETERSSTATE_H
#define PARAMETERSSTATE_H

#include <TGUI/Scrollbar.hpp>
#include <TGUI/Checkbox.hpp>
#include <TGUI/ComboBox.hpp>
#include <TGUI/Canvas.hpp>
#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/Grid.hpp>

#include <TheLostGirl/State.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}

class StateStack;

/// State that allow the user to change settings of the game.
class ParametersState : public State
{
	public:
        /// Constructor.
        /// \param stack StateStack wherein the State is added.
		ParametersState(StateStack& stack);

		/// Destructor.
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
		tgui::Panel::Ptr m_background;           ///< The background of the menu.
		tgui::Label::Ptr m_title;                ///< The title label of the menu
		tgui::Label::Ptr m_resolutionLabel;      ///< The label of the resolution combo box.
		tgui::ComboBox::Ptr m_resolutionComboBox;///< The resolution combo box.
		tgui::Label::Ptr m_langLabel;            ///< The label of the lang combo box.
		tgui::ComboBox::Ptr m_langComboBox;      ///< The lang combo box.
		tgui::Label::Ptr m_bloomLabel;           ///< The label of the bloom checkbox.
		tgui::Checkbox::Ptr m_bloomCheckbox;     ///< The bloom checkbox.
		tgui::Button::Ptr m_applyButton;         ///< The apply button, at the bottom.
		tgui::Button::Ptr m_cancelButton;        ///< The cancel button, at the bottom.
		tgui::Button::Ptr m_okButton;            ///< The ok button, at the bottom.

		/// Pop the state and return to the pause menu.
		void backToPause();

		/// Apply the new settings.
		void applyChanges();

		/// Gives the string corresponding to the given Lang, to display in parameters menu.
		/// \param lang The lang to convert.
		/// \return The corresponding string.
		std::string toString(Lang lang);

		/// Gives the Lang corresponding to the given string.
		/// \param string The string to convert.
		/// \return The corresponding Lang.
		Lang fromString(std::string string);
};

#endif//PARAMETERSSTATE_H

