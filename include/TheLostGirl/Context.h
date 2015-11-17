#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>

//Forward declarations
namespace sf
{
       class RenderWindow;
       class RenderTexture;
       class Font;
       class Texture;
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

struct Parameters;
template <typename Resource, typename Identifier>
class ResourceManager;
class Interpreter;
typedef ResourceManager<sf::Texture, std::string> TextureManager;
typedef ResourceManager<sf::Font, std::string> FontManager;
typedef ResourceManager<Interpreter, std::string> ScriptManager;
class b2World;
class Player;
class LangManager;
class StateStack;

/// Various managers and instances references.
/// It is useful to pass the context to the states,
/// they can then use these various resources managers.
struct Context
{
	static Parameters* parameters;               ///< Structure containing all the game parameters.
	static sf::RenderWindow* window;             ///< The main window.
	static sf::RenderTexture* postEffectsTexture;///< Texture for rendering posteffects.
	static TextureManager* textureManager;       ///< The textures manager.
	static FontManager* fontManager;             ///< The fonts manager.
	static ScriptManager* scriptManager;         ///< The scripts manager.
	static LangManager* langManager;             ///< The lang manager.
	static tgui::Gui* gui;                       ///< The main GUI manager.
	static entityx::EventManager* eventManager;  ///< The event manager of the entity system.
	static entityx::EntityManager* entityManager;///< The entity manager of the entity system
	static entityx::SystemManager* systemManager;///< The system manager of the entity system
	static b2World* world;                       ///< The Box2D physic world.
	static Player* player;                       ///< The input manager.
	static StateStack* stateStack;               ///< The state stack.
};

#endif//CONTEXT_H
