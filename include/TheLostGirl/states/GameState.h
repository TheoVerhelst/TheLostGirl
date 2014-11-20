#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <forward_list>
#include <thread>

#include <dist/json/json-forwards.h>
#include <SFML/System/Thread.hpp>
#include <Box2D/Dynamics/Joints/b2Joint.h>

#include <SFML/Graphics/Sprite.hpp>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/ContactListener.h>
#include <TheLostGirl/ContactFilter.h>

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
        /// \param context Current context of the application.
		GameState(StateStack& stack, Context context);
		
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
		struct Joint
		{
			b2JointDef* definition;
			b2JointType type;
			std::string entityA;
			std::string entityB;
			std::string partA;
			std::string partB;
		};
		
		struct SceneReplaces
		{
			sf::IntRect origin;
			std::vector<Transform> replaces;
		};
		
		/// Initialize the physic world.
		/// \param file Path of the save file to load.
		void initWorld(const std::string& file);
		
		/// Save the physic world.
		/// \param file Path of the save file to override.
		void saveWorld(const std::string& file);
		
		//A unordered map allow to reference to elements of the list safely, and use those references outside the scope of the GameState class.
		std::unordered_map<std::string, entityx::Entity> m_entities;          ///< All game entities.
		std::unordered_map<std::string, entityx::Entity> m_sceneEntities;     ///< All scene entities.
		std::map<std::string, std::vector<SceneReplaces>> m_sceneEntitiesData;///< A map containing data about scene entities.
		std::vector<Joint> m_joints;                                          ///< A vector containing all informations about joints.
		ContactListener m_contactListener;                                    ///< The contact listener.
		ContactFilter m_contactFilter;                                        ///< The contact filter.
		float m_timeSpeed;                                                    ///< The speed of the time (usually 1.f). This influes only on the TimeSystem, not on physics!
		bool m_loadingFinished;                                               ///< Indicate if the level loading is finished.
		std::thread m_threadLoad;                                             ///< Thread launched when loading the level.
		
		                                                                     //Level informations
		std::string m_levelIdentifier;                                        ///< Identifer of the level, must be a non-spaced name.
		unsigned short int m_numberOfPlans;                                   ///< Number of plans in the background.
		float m_referencePlan;                                                ///< Number of the plan where actors evolute.
		sf::IntRect m_levelRect;                                              ///< The dimensions of the level, in pixels.
};

#endif // GAMESTATE_H
