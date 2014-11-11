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

/// State that load all the ressources that need to be loaded in order to play a level.
class LoadingState : public State, public entityx::Receiver<LoadingState>
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param context Context of the game.
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
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);
		
		void receive(const LoadingStateChange &loadingStateChange);

	private:
		tgui::Panel::Ptr m_background;
		tgui::Label::Ptr m_sentenceLabel;
		std::wstring m_sentence;
		tgui::Label::Ptr m_percentLabel;
};

#endif // LOADINGSTATE_H
