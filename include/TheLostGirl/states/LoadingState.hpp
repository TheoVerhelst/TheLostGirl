#ifndef LOADINGSTATE_HPP
#define LOADINGSTATE_HPP

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
class LoadingStateChange;
struct ParametersChange;

/// State that display the current state of the loading.
class LoadingState : public State
{
	public:
		/// Constructor.
		LoadingState();

		/// Destructor.
		~LoadingState();

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

		/// Receive an event indicating that the loading has progressed.
		/// \param loadingStateChange Structure containing data about the change.
		void receive(const LoadingStateChange& loadingStateChange);

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

		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		tgui::Panel::Ptr m_background;   ///< The grey background.
		std::string m_sentence;          ///< String explaining the loading state.
		bool m_sentenceChanged;          ///< Indicates whether the sentence has changed and need to be updated.
		tgui::Label::Ptr m_sentenceLabel;///< Label explaining the loading state.
		std::string m_hint;              ///< Text of the hint, not translated.
		tgui::Label::Ptr m_hintLabel;    ///< Label indicating a random hint.
		float m_sentenceTimer;           ///< Timer in seconds of a little animation on the sentence.
};

#endif//LOADINGSTATE_HPP
