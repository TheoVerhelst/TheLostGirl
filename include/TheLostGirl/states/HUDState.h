#ifndef HUDSTATE_H
#define HUDSTATE_H

#include <TGUI/Canvas.hpp>

#include <TheLostGirl/HashEntity.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class State;
class Context;
class StateStack;
struct EntityHealthChange;
struct EntityStaminaChange;

/// State that display data about the player.
class HUDState : public State
{
	public:
        /// Constructor.
        /// The role of the entitiesNumber parameter is to load some tgui::Canvas
        /// in order to do not load them on the fly during the game, when entities health change.
        /// \param stack StateStack wherein the State is added.
        /// \param entitiesNumber Number of entities that will be in the level.
		HUDState(StateStack& stack, size_t entitiesNumber);

		/// Destructor.
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

		/// Receive an event indicating that the health of an entity changed.
		/// \param entityHealthChange Structure containing data about the change.
		void receive(const EntityHealthChange& entityHealthChange);

		/// Receive an event indicating that the health of an entity changed.
		/// \param entityStaminaChange Structure containing data about the change.
		void receive(const EntityStaminaChange& entityStaminaChange);

	private:
		/// Holds graphical data to represent a bar for each entity with a given stat.
		/// This can be health, stamina, mana, and so on.
		/// A fading to transparent is done when the entity's stat is full.
		struct Bar
		{
			/// Constructor.
			Bar()
			{}

			/// Destructor.
			~Bar()
			{}

			tgui::Canvas::Ptr canvas;///< Canvas to draw the sprites.
			sf::Sprite sprite;       ///< The sprite of the bar.
			sf::Sprite borderSprite; ///< The sprite of the borders of the bar.
			bool isFull;             ///< Indicates whether the bar is full.
			sf::Time timer;          ///< Timer of the fading animation.
		};

		std::unordered_map<entityx::Entity, Bar, HashEntity> m_healthBars; ///< Store all entities health bars.
		std::unordered_map<entityx::Entity, Bar, HashEntity> m_staminaBars;///< Store all entities stamina bars.
		std::list<tgui::Canvas::Ptr> m_loadedCanvas;              ///< Store some canvas that will be assigned to an entity later.
		tgui::Canvas::Ptr m_playerHealthCanvas;                            ///< A preloaded canvas for the player health bar.
		tgui::Canvas::Ptr m_playerStaminaCanvas;                           ///< A preloaded canvas for the player stamina bar.

		//Player wind HUD
		tgui::Canvas::Ptr m_windBar;       ///< The wind arrow.
		sf::Sprite m_windStrengthSpr;      ///< The sprite of the wind arrow.
		sf::Sprite m_windStrengthBarSpr;   ///< The sprite of the wind bar.
		bool m_windIsFading;               ///< Indicates whether the fading of the wind arrow is currently active.
		sf::Time m_windFadingTimer;        ///< Timer of the wind arrow fading animation.
};

#endif//HUDSTATE_H
