#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <ctime>
#include <memory>
#include <cstdlib>

//Unable to forward-declare those types
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/StateStack.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
	class SystemManager;
}

///Main game class.
///That class holds all the components needed to execute the game (states stack, entity manager, physic engine, ...).
class Application
{
	public:
		Application();
		~Application();
		
        ///Initialize the game.
        /// \return 0 if succeful, -1 otherwise.
        ///Function that do all that need to be done at the beginning of the game.                                
		int init();
		
        ///Run the game.
        /// \return 0 if succeful, -1 otherwise.
        ///The function contains the main game loop.
		int run();

	private:
		void processInput();
		void update(sf::Time dt);
		void render();
		void registerStates();

		sf::RenderWindow m_window;//The main window
		tgui::Gui m_gui;//All the gui
		TextureManager m_textureManager;//The texture manager
		FontManager m_fontManager;//The fonts manager
		entityx::EventManager m_eventManager;//The entity events manager
		entityx::EntityManager m_entityManager;//The entity manager
		entityx::SystemManager m_systemManager;//The entity systems manager
		b2Vec2 m_gravity;//The gravity vector
		b2World m_world;//The Box2D world
		StateStack m_stateStack;//The game state manager
};

#endif // APPLICATION_H
