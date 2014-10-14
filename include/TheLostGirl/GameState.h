#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics/Sprite.hpp>
#include <TheLostGirl/Animations.h>
//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
namespace entityx
{
	class Entity;
}
class State;
class Context;
class StateStack;

/// State that effectively play the game.
/// The game become really interesting here.
class GameState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack in where the State is added.
        /// \param context Context of the game.
		GameState(StateStack& stack, Context context);
		
        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// \param dt Elapsed time since the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);

	private:
		entityx::Entity m_archer;      ///< Main character.
		sf::Sprite m_archerSprite;     ///< The sprite of the archer.
		Animations m_archerAnimations; ///< The animations of the archer.
};

#endif // GAMESTATE_H
