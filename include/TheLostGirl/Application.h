#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <ctime>
#include <memory>
#include <cstdlib>
#include <queue>

//Unable to forward-declare those types
#include <TGUI/Gui.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/DebugDraw.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
	class SystemManager;
}

/// Main game class.
/// That class holds all the components needed to execute the game (states stack, entity manager, physics engine, ...).
class Application
{
	public:
		/// Default constructor.
		/// \param debugMode Indicate if the appliaction should start in debug mode.
		Application(bool debugMode = false);
		
		/// Default destructor.
		~Application();
		
        /// Initialize the game.
        /// This does all that need to be done at the beginning of the game. 
        /// \return 0 on succes, 1 on failure.                               
		int init();
		
        /// Run the game.
        /// This function contains the main game loop.
        /// \return 0 on succes, 1 on undefined program failure or 2 on level loading failure.
		int run();

	private:
		/// Various input handling.
		void processInput();     
		
		/// Updates logics, systems, and so on.
        /// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);
		
		/// Render all that need to be displayed on the screen.
		void render();
		
		/// Registers all the logic systems.
		void registerSystems();
		
		Parameters m_parameters;               ///< Structure containing all the game parameters.
		sf::RenderWindow m_window;             ///< The main window.
		tgui::Gui m_gui;                       ///< All the gui.
		TextureManager m_textureManager;       ///< The texture manager.
		FontManager m_fontManager;             ///< The fonts manager.
		entityx::EventManager m_eventManager;  ///< The entity events manager.
		entityx::EntityManager m_entityManager;///< The entity manager.
		entityx::SystemManager m_systemManager;///< The entity systems manager.
		Player m_player;                       ///< The input manager.
		b2World m_world;                       ///< The Box2D world.
		StateStack m_stateStack;               ///< The game state manager.
		DebugDraw m_debugDraw;                 ///< The debugging drawer.
};

#endif//APPLICATION_H
