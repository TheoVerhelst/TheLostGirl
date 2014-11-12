#ifndef HUDSTATE_H
#define HUDSTATE_H

#include <TGUI/Label.hpp>
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
class PlayerHealthChange;
class PlayerStaminaChange;

/// State that display data about the player.
class HUDState     : public State, public entityx::Receiver<HUDState>
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param context Current context of the application.
		HUDState(StateStack& stack, Context context);
		
		/// Default destructor.
		~HUDState();
		
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
		
		/// Receive an event indicating that the health of the player changed.
		/// \param playerStatsChange Structure containing data about the change. 
		void receive(const PlayerHealthChange& playerHealthChange);
		
		/// Receive an event indicating that the health of the player changed.
		/// \param playerStatsChange Structure containing data about the change. 
		void receive(const PlayerStaminaChange& playerStaminaChange);

	private:
		tgui::Label::Ptr m_healthLabel; ///< Label indicating the current heath of the player.
		tgui::Label::Ptr m_staminaLabel;///< Label indicating the current stamina of the player.
};

#endif // HUDSTATE_H
