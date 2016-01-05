#ifndef PARAMETERSSTATE_H
#define PARAMETERSSTATE_H

#include <TGUI/TGUI.hpp>
#include <TheLostGirl/State.hpp>
#include <TheLostGirl/LangManager.hpp>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}

class StateStack;
struct ParametersChange;

/// State that allow the user to change settings of the game.
class ParametersState : public State
{
	public:
        /// Constructor.
		ParametersState();

		/// Destructor.
		~ParametersState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() override;

        /// The logic update function.
        /// This function call e.g. the physic update function, the AI function, etc...
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
		virtual bool update(sf::Time dt) override;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) override;

		/// Receive an event about a change in the parameters.
		/// This function is defined because this is possible that
		/// this is not this state wich change parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

	private:
		/// Apply the new settings.
		void applyChanges();

		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::Panel::Ptr m_background;           ///< The background of the menu.
		tgui::Label::Ptr m_title;                ///< The title label of the menu

		//Options
		tgui::Label::Ptr m_langLabel;            ///< The label of the lang combo box.
		tgui::ComboBox::Ptr m_langComboBox;      ///< The lang combo box.
		tgui::Label::Ptr m_bloomLabel;           ///< The label of the bloom checkbox.
		tgui::CheckBox::Ptr m_bloomCheckBox;     ///< The bloom checkbox.
		tgui::Label::Ptr m_mainVolumeLabel;      ///< The label of the main volume slider.
		tgui::Slider::Ptr m_mainVolumeSlider;    ///< The main volume slider.
		tgui::Label::Ptr m_musicVolumeLabel;     ///< The label of the music volume slider.
		tgui::Slider::Ptr m_musicVolumeSlider;   ///< The music volume slider.
		tgui::Label::Ptr m_effectsVolumeLabel;   ///< The label of the effects volume slider.
		tgui::Slider::Ptr m_effectsVolumeSlider; ///< The effects volume slider.
		tgui::Label::Ptr m_ambianceVolumeLabel;  ///< The label of the ambiance volume slider.
		tgui::Slider::Ptr m_ambianceVolumeSlider;///< The ambiance volume slider.
		tgui::Label::Ptr m_fullscreenLabel;      ///< The fulscreen label.
		tgui::CheckBox::Ptr m_fullscreenCheckBox;///< The fullscreen checkbox.
		tgui::ComboBox::Ptr m_fullscreenComboBox;///< The fulscreen combo box.
		tgui::Label::Ptr m_controlsLabel;        ///< The label of the controls button.
		tgui::Button::Ptr m_controlsButton;      ///< The controls button.

		//Buttons
		tgui::Button::Ptr m_applyButton;         ///< The apply button, at the bottom.
		tgui::Button::Ptr m_cancelButton;        ///< The cancel button, at the bottom.
		tgui::Button::Ptr m_okButton;            ///< The ok button, at the bottom.
};

#endif//PARAMETERSSTATE_H

