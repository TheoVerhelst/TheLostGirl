#ifndef INTROSTATE_HPP
#define INTROSTATE_HPP

#include <TGUI/TGUI.hpp>
#include <TheLostGirl/State.hpp>

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
class IntroState : public State
{
	public:
        /// Constructor.
		IntroState();

		/// Destructor.
		~IntroState();

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
		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::Panel::Ptr m_background; ///< The background of the menu.
		tgui::Picture::Ptr m_logo;     ///< The Lost Girl logo.
		tgui::Label::Ptr m_sentence;   ///< The sentence under the logo.
};

#endif//INTROSTATE_HPP
