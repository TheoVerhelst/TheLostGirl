#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <TGUI/TGUI.hpp>
#include <SFML/Audio.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/ResourceManager.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/BloomEffect.hpp>
#include <TheLostGirl/StateStack.hpp>
#include <TheLostGirl/Player.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/scripts/Interpreter.hpp>
#include <TheLostGirl/DebugDraw.hpp>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/DestructionListener.hpp>

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
		/// \param debugMode Indicate if the appliaction starts in debug mode.
		Application(bool debugMode = false);

		/// Destructor.
		~Application();

        /// Run the game.
        /// This function contains the main game loop.
		void run();

		/// Receive an event about a change in the parameters.
		/// \param changes The data about the change.
		void receive(const ParametersChange& changes);

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

		/// Load the settings in the settings file and apply them.
		void deserializeSettings();

		/// Calculate a new view for the new size of the window.
		/// Then apply this view to the window or the bloom texture,
		/// depending of whether the bloom is enabled, and to the gui.
		/// The goal is keeping a 16:9 display ratio for the window view.
		void handleResize();

		friend Context;

		Parameters m_parameters;                  ///< Structure containing all the game parameters.
		sf::RenderWindow m_window;                ///< The main window.
		sf::RenderTexture m_postEffectsTexture;   ///< Texture for rendering post effects.
		ResourceManager<sf::Texture, std::string> m_textureManager;///< The texture manager.
		ResourceManager<sf::Font, std::string> m_fontManager;///< The fonts manager.
		ResourceManager<Interpreter, std::string> m_scriptManager;///< The scripts manager.
		ResourceManager<sf::SoundBuffer, std::string> m_soundManager;///< The sounds manager.
		LangManager m_langManager;                ///< The langs manager.
		tgui::Gui m_gui;                          ///< All the gui.
		entityx::EventManager m_eventManager;     ///< The entity events manager.
		entityx::EntityManager m_entityManager;   ///< The entity manager.
		entityx::SystemManager m_systemManager;   ///< The entity systems manager.
		b2World m_world;                          ///< The Box2D world.
		Player m_player;                          ///< The input manager.
		StateStack m_stateStack;                  ///< The game state manager.
		Context m_context;                        ///< This member initializes the global variables of the context.
		DebugDraw m_debugDraw;                    ///< The debugging drawer.
		DestructionListener m_destructionListener;///< The fixture/joint destruction handler.
		BloomEffect m_bloomEffect;                ///< The shader to apply on m_postEffectsTexture.
		sf::Time m_FPSTimer;                      ///< Count time since last FPS ath update.
		sf::Time m_FPSRefreshRate;                ///< Refresh rate of the FPS ath.
		sf::Time m_frameTime;                     ///< The target frame time (1/FPS).
};

#endif//APPLICATION_HPP
