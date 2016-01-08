#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <string>

//Forward declarations
namespace sf
{
   class RenderWindow;
   class RenderTexture;
   class Font;
   class Texture;
   class SoundBuffer;
}

namespace tgui
{
   class Gui;
}

namespace entityx
{
   class EventManager;
   class EntityManager;
   class SystemManager;
}

class Application;
struct Parameters;
template <typename Resource, typename Identifier>
class ResourceManager;
class Interpreter;
class b2World;
class Player;
class LangManager;
class StateStack;

/// Various managers and instances references.
/// It is useful to pass the context to the states,
/// they can then use these various resources managers.
struct Context
{
	/// Constructor.
	/// Initialize the global variable according to the given Application instance.
	/// \param application The main Application instance.
	explicit Context(Application& application);

	static Parameters* parameters;               ///< Structure containing all the game parameters.
	static sf::RenderWindow* window;             ///< The main window.
	static sf::RenderTexture* postEffectsTexture;///< Texture for rendering posteffects.
	static ResourceManager<sf::Texture, std::string>* textureManager;       ///< The textures manager.
	static ResourceManager<sf::Font, std::string>* fontManager;             ///< The fonts manager.
	static ResourceManager<sf::SoundBuffer, std::string>* soundManager;           ///< The sounds manager.
	static ResourceManager<Interpreter, std::string>* scriptManager;         ///< The scripts manager.
	static LangManager* langManager;             ///< The langs manager.
	static tgui::Gui* gui;                       ///< The main GUI manager.
	static entityx::EventManager* eventManager;  ///< The event manager of the entity system.
	static entityx::EntityManager* entityManager;///< The entity manager of the entity system
	static entityx::SystemManager* systemManager;///< The system manager of the entity system
	static b2World* world;                       ///< The Box2D physic world.
	static Player* player;                       ///< The input manager.
	static StateStack* stateStack;               ///< The state stack.
};

#endif//CONTEXT_HPP
