#ifndef APPLICATION_H
#define APPLICATION_H

//Unable to forward-declare those types
#include <TGUI/Gui.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>

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

struct ParametersChange;

/// Main game class.
/// That class holds all the components needed to execute the game (states stack, entity manager, physics engine, ...).
class Application : public entityx::Receiver<Application>
{
	public:
		/// Constructor.
		/// \param debugMode Indicate if the appliaction should start in debug mode.
		Application(bool debugMode = false);

		/// Destructor.
		~Application();

        /// Initialize the game.
        /// This does all that need to be done at the beginning of the game.
        /// \return 0 on succes, 1 on failure.
		int init();

        /// Run the game.
        /// This function contains the main game loop.
        /// \return 0 on succes, 1 on undefined program failure or 2 on level loading failure.
		int run();

		/// Receive an event about a change in the parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

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
		sf::RenderTexture m_postEffectsTexture;///< Texture for rendering posteffects.
		tgui::Gui m_gui;                       ///< All the gui.
		TextureManager m_textureManager;       ///< The texture manager.
		FontManager m_fontManager;             ///< The fonts manager.
		ScriptManager m_scriptManager;         ///< The scripts manager.
		LangManager m_langManager;             ///< The lang manager.
		entityx::EventManager m_eventManager;  ///< The entity events manager.
		entityx::EntityManager m_entityManager;///< The entity manager.
		entityx::SystemManager m_systemManager;///< The entity systems manager.
		b2World m_world;                       ///< The Box2D world.
		StateStack m_stateStack;               ///< The game state manager.
		Player m_player;                       ///< The input manager.
		DebugDraw m_debugDraw;                 ///< The debugging drawer.
		sf::Time m_FPSTimer;                   ///< Count time since last FPS ath update.
		sf::Time m_FPSRefreshRate;             ///< Refresh rate of the FPS ath.
		sf::Time m_frameTime;                  ///< The target frame time (1/FPS).
		unsigned int m_newScaleIndex;          ///< The scale index that will be saved in the settings file at the end of the application.
};

#endif//APPLICATION_H
