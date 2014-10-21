#ifndef STATE_H
#define STATE_H

#include <memory>
#include <queue>

#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/StateIdentifiers.h>

namespace sf
{
	class Event;
	class Time;
	class RenderWindow;
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
class b2World;
class State;
class StateStack;
class Player;
class Command;
typedef std::queue<Command> CommandQueue;

/// Base class for the various game states.
/// In order to make a game state, you must inherit from this class and implement the pure virtal functions.
/// The new state must then be registered and pushed in a StateStack.
class State
{
	public:
        /// Pointer typedef.
		typedef std::unique_ptr<State> Ptr;

        /// Various managers and globals instances.
        /// It is useful to pass the context to the states,
        /// they can then use these various ressources managers.
		struct Context
		{
			/// Default constructor
			/// \param _window The main window
			/// \param _textureManager The texture manager
			/// \param _fontManager The font manager
			/// \param _gui The main GUI manager
			/// \param _eventManager The event manager of the entity system.
			/// \param _entityManager The entity manager of the entity system.
			/// \param _systemManager The system manager of the entity system.
			/// \param _world The Box2D physic world.
			/// \param _player The input manager.
			/// \param _commandQueue The queue of commands.
			Context(sf::RenderWindow& _window,
					TextureManager& _textureManager,
					FontManager& _fontManager,
					tgui::Gui& _gui,
					entityx::EventManager& _eventManager,
					entityx::EntityManager& _entityManager,
					entityx::SystemManager& _systemManager,
					b2World& _world,
					Player& _player,
					CommandQueue& _commandQueue
				);
			sf::RenderWindow& window;             ///< The main window
			TextureManager& textureManager;       ///< The texture manager
			FontManager& fontManager;             ///< The font manager
			tgui::Gui& gui;                       ///< The main GUI manager
			entityx::EventManager& eventManager;  ///< The event manager of the entity system.
			entityx::EntityManager& entityManager;///< The entity manager of the entity system
			entityx::SystemManager& systemManager;///< The system manager of the entity system
			b2World& world;                       ///< The Box2D physic world.
			Player& player;                       ///< The input manager.
			CommandQueue& commandQueue;           ///< The queue of commands.
		};

        /// Default constructor.
        /// \param stack StateStack in where the State is added.
        /// \param context Context of the game.
		State(StateStack& stack, Context context);
		virtual ~State();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() = 0;

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt) = 0;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// The state must decide what to do with the event.
        /// The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) = 0;

	protected:
		/// Add the given game state to the pending queue,
		/// and construct it as soon as possible.
		/// \param stateID Identifier of the state to push.
		void requestStackPush(States stateID);
		
		/// Delete the top state as soon as possible.
		void requestStackPop();
		
		/// Delete all the states as soon as possible.
		void requestStateClear();
		
		/// Give the context of the application.
		/// \return Context structure of the current game.
		Context getContext() const;

	private:
		StateStack* m_stateStack;///< Pointer to the state stack that holds this state.
		Context m_context;///< Current context of the game.
};

#endif // STATE_H