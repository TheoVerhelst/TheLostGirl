#ifndef LOADINGSTATE_H
#define LOADINGSTATE_H

#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
#include <entityx/Event.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class State;
class Context;
class StateStack;
class LoadingStateChange;

/// State that display the current state of the loading.
class LoadingState : public State, public entityx::Receiver<LoadingState>
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param context Current context of the application.
		LoadingState(StateStack& stack, Context context);
		
		/// Default destructor.
		~LoadingState();
		
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
		
		/// Receive an event indicating that the loading has progressed.
		/// \param loadingStateChange Structure containing data about the change. 
		void receive(const LoadingStateChange& loadingStateChange);

	private:
		tgui::Panel::Ptr m_background;   ///< The grey background.
		tgui::Label::Ptr m_sentenceLabel;///< Label explaining the loading state.
		std::wstring m_sentence;         ///< String explaining the loading state.
		tgui::Label::Ptr m_hintLabel;    ///< Label indicating a random hint.
		double m_sentenceTimer;          ///< Timer in seconds of a little animation on the sentence.
};

#endif//LOADINGSTATE_H
