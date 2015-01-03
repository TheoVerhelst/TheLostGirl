#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <ctime>
#include <memory>
#include <cstdlib>
#include <queue>

//Unable to forward-declare those types
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/ResourceManager.h>
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
/// That class holds all the components needed to execute the game (states stack, entity manager, physic engine, ...).
class Application
{
	public:
		/// Default constructor.
		/// \param debugMode Indicate if the appliaction should start in debug mode.
		Application(bool debugMode = false);
		
		/// Default destructor.
		~Application();
		
        /// Initialize the game.
        /// \return 0 if succeful, -1 otherwise.
        /// This does all that need to be done at the beginning of the game.                                
		int init();
		
        /// Run the game.
        /// \return 0 if succeful, -1 otherwise.
        /// This function contains the main game loop.
		int run();

	private:
		void processInput();     ///< Various input handling.
		
		/// Updates logics, systems, and so on.
        /// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);
		
		void render();           ///< Render all that need to be displayed on the screen.
		void registerStates();   ///< Registers all the game states.
		void registerSystems();  ///< Registers all the logic systems.
		
		Parameters m_parameters;               ///< Structure containing all the game parameters.
		sf::RenderWindow m_window;             ///< The main window.
		sf::RenderTexture m_texture;           ///< The main texture.
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
