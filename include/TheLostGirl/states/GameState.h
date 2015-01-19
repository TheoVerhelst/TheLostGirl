#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <thread>

#include <Box2D/Dynamics/Joints/b2Joint.h>

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
class GameState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
		GameState(StateStack& stack);
		
		/// Default destructor.
		/// Remove all bodies, sprites and others from the memory.
		~GameState();
		
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
		/// Structure that hold data about scene entities.
		/// The role of this texture is to hold data that cannot easily be retrieved in the EntityManager instance.
		struct SceneReplaces
		{
			sf::IntRect origin;             ///< Coordinate of the texture to place in the world.
			std::vector<Transform> replaces;///< List of places where the texture should be displayed.
		};
		
		/// Initialize the physic world.
		/// \param file Path of the save file to load.
		void initWorld(const std::string& file);
		
		/// Save the physic world.
		/// \param file Path of the save file to override.
		void saveWorld(const std::string& file);
		
		std::map<std::string, entityx::Entity> m_entities;                    ///< All game entities.
		std::map<std::string, entityx::Entity> m_sceneEntities;               ///< All scene entities.
		std::map<std::string, std::vector<SceneReplaces>> m_sceneEntitiesData;///< A map containing data about scene entities.
		ContactListener m_contactListener;                                    ///< The contact listener.
		float m_timeSpeed;                                                    ///< The speed of the time (usually 1.f). This influes only on the TimeSystem, not on physics!
		std::thread m_threadLoad;                                             ///< Thread launched when loading the level.
		bool m_loading;                                                       ///< Indicates wether the loading is finished or not.
		
		//Level informations
		std::string m_levelIdentifier;                                        ///< Identifer of the level, must be a non-spaced name.
		unsigned short int m_numberOfPlans;                                   ///< Number of plans in the background.
		float m_referencePlan;                                                ///< Number of the plan where actors evolute.
		sf::IntRect m_levelRect;                                              ///< The dimensions of the level, in pixels.
};

#endif//GAMESTATE_H
