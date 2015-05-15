#ifndef EMPTYLEVELSTATE_H
#define EMPTYLEVELSTATE_H

#include <thread>

#include <SFML/Graphics/Sprite.hpp>

#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/ContactListener.h>

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
struct Transform;

/// State that effectively play the game.
/// The game become really interesting here.
class EmptyLevelState : public State
{
	public:
        /// Constructor.
        /// \param stack StateStack wherein the State is added.
		EmptyLevelState(StateStack& stack);

		/// Destructor.
		/// Remove all bodies, sprites and others from the memory.
		~EmptyLevelState();

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

	private:
		/// Initialize the physic world.
		/// \param file Path of the save file to load.
		void initWorld(const std::string& file);

		std::map<std::string, entityx::Entity> m_entities;     ///< All game entities.
		std::map<std::string, entityx::Entity> m_sceneEntities;///< All scene entities.
		float m_timeSpeed;                                     ///< The speed of the time (usually 1.f). This influes only on the TimeSystem, not on physics!
		std::thread m_threadLoad;                              ///< Thread launched when loading the level.

		//Level informations
		std::string m_levelIdentifier;                         ///< Identifer of the level, must be a non-spaced name.
		unsigned int m_numberOfPlans;                          ///< Number of plans in the background.
		float m_referencePlan;                                 ///< Number of the plan where actors evolute.
		sf::IntRect m_levelRect;                               ///< The dimensions of the level, in pixels.
};

#endif//EMPTYLEVELSTATE_H
