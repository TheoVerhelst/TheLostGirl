#ifndef STATESTACK_H
#define STATESTACK_H

#include <vector>
#include <memory>

#include <SFML/System/NonCopyable.hpp>

namespace sf
{
	class Event;
	class Time;
	class RenderWindow;
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
typedef ResourceManager<sf::Texture, std::string> TextureManager;
typedef ResourceManager<sf::Font, std::string> FontManager;
class b2World;
class Player;
struct State;

/// States manager.
/// This class holds all the game or menu state and manages them.
/// \see State
class StateStack : private sf::NonCopyable
{
	public:
        /// Various managers and globals instances.
        /// It is useful to pass the context to the states,
        /// they can then use these various resources managers.
		struct Context
		{
			/// Default constructor
			/// \param _parameters Structure containing all the game parameters.
			/// \param _window The main window.
			/// \param _textureManager The texture manager.
			/// \param _fontManager The font manager.
			/// \param _gui The main GUI manager.
			/// \param _eventManager The event manager of the entity system.
			/// \param _entityManager The entity manager of the entity system.
			/// \param _systemManager The system manager of the entity system.
			/// \param _world The Box2D physic world.
			/// \param _player The input manager.
			Context(Parameters& _parameters,
					sf::RenderWindow& _window,
					TextureManager& _textureManager,
					FontManager& _fontManager,
					tgui::Gui& _gui,
					entityx::EventManager& _eventManager,
					entityx::EntityManager& _entityManager,
					entityx::SystemManager& _systemManager,
					b2World& _world,
					Player& _player
				);
			Parameters& parameters;               ///< Structure containing all the game parameters.
			sf::RenderWindow& window;             ///< The main window.
			TextureManager& textureManager;       ///< The texture manager.
			FontManager& fontManager;             ///< The font manager.
			tgui::Gui& gui;                       ///< The main GUI manager.
			entityx::EventManager& eventManager;  ///< The event manager of the entity system.
			entityx::EntityManager& entityManager;///< The entity manager of the entity system
			entityx::SystemManager& systemManager;///< The system manager of the entity system
			b2World& world;                       ///< The Box2D physic world.
			Player& player;                       ///< The input manager.
		};
		
		/// Default constructor.
		/// \param context Current context of the application.
		/// \see StateStack::Context
		explicit StateStack(StateStack::Context context);
		
		/// Default destructor;
		~StateStack();
		
		/// Call the update() function of all the states in the stack.
		/// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);
		
		/// Call the draw() function of all the states in the stack.
		void draw();
		
		/// Call the handleEvent() function of all the states in the stack.
		/// \param event SFML event to handle.
		void handleEvent(const sf::Event& event);
		
		/// Add a new state of the templated type on the pending list, and construct it as soon as possible.
		/// \param args Arguments that will be forwarded to the constructor of the state.
		/// There can be zero, one or more arguments of any types.
		template <typename T, typename ... Args>
		void pushState(Args&&... args);
		
		/// Delete the top state as soon as possible.
		void popState();
		
		/// Delete all the states as soon as possible.
		void clearStates();
		
		/// Check if the stack is empty or not.
		/// \return True if the stack is empty, false otherwise.
		bool isEmpty() const;
		
		/// Acces the current context.
		/// \return The current context.
		Context getContext();

	private:
		/// Remove states that need to be removed, constructs others states, ...
		void applyPendingChanges();
		
		std::vector<std::unique_ptr<State>> m_stack;     ///< Array of pointers to the states.
		std::vector<std::function<void()>> m_pendingList;///< List of all changes to do in the next update.
		Context m_context;                               ///< Context of the application.
};

template <typename T, typename ... Args>
void StateStack::pushState(Args&&... args)
{
	m_pendingList.push_back([&, this, args...]{m_stack.push_back(std::unique_ptr<State>(new T(*this, std::forward<Args>(args)...)));});
}

#endif//STATESTACK_H
