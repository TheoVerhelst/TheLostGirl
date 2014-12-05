#ifndef HUDSTATE_H
#define HUDSTATE_H

#include <TGUI/Canvas.hpp>
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
struct PlayerHealthChange;
struct PlayerStaminaChange;

/// State that display data about the player.
class HUDState : public State, public entityx::Receiver<HUDState>
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
		/// \param playerHealthChange Structure containing data about the change. 
		void receive(const PlayerHealthChange& playerHealthChange);
		
		/// Receive an event indicating that the health of the player changed.
		/// \param playerStaminaChange Structure containing data about the change. 
		void receive(const PlayerStaminaChange& playerStaminaChange);

	private:
		//Canvas
		tgui::Canvas::Ptr m_healthBar;   ///< The health bar.
		tgui::Canvas::Ptr m_staminaBar;  ///< The stamina bar.
		tgui::Canvas::Ptr m_windBar;     ///< The wind arrow.
		//Sprites
		sf::Sprite m_healthSpr;          ///< The sprite of the health bar.
		sf::Sprite m_healthBorderSpr;    ///< The sprite of the health bar border.
		sf::Sprite m_staminaSpr;         ///< The sprite of the stamina bar.
		sf::Sprite m_staminaBorderSpr;   ///< The sprite of the stamina bar border.
		sf::Sprite m_windStrengthSpr;    ///< The sprite of the wind arrow.
		sf::Sprite m_windStrengthBarSpr; ///< The sprite of the wind bar.
		//Fading data
		bool m_healthIsFading;             ///< True only if the fading of the health bar is currently active.
		bool m_staminaIsFading;            ///< True only if the fading of the stamina bar is currently active.
		bool m_windIsFading;               ///< True only if the fading of the wind arrow is currently active.
		sf::Clock m_healthFadingTimer;     ///< Timer of the health bar fading.
		sf::Clock m_staminaFadingTimer;    ///< Timer of the stamina bar fading.
		sf::Clock m_windFadingTimer;       ///< Timer of the wind arrow fading.
};

#endif//HUDSTATE_H
