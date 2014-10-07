#ifndef STATE_H
#define STATE_H

#include <memory>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>

#include <TheLostGirl/StateIdentifiers.h>
#include <TheLostGirl/ResourceIdentifiers.h>

namespace sf
{
class RenderWindow;
}

class StateStack;

using entityx::EventManager;
using entityx::EntityManager;
using entityx::SystemManager;

///Base class for the various game states.
///Inherits from this class and implements the pure virtal functions.
///The New state must then be registered and pushed to the StateStack.
class State
{
	public:
        ///Convenient typedef.
		typedef std::unique_ptr<State> Ptr;

        ///Various managers and globals instances.
        ///It is useful to pass the context to the states,
        ///they can then use these various ressources managers.
		struct Context
		{
			Context(sf::RenderWindow& _window,
					TextureManager& _textures,
					FontManager& _fonts,
					tgui::Gui& _gui,
					EventManager& _eventManager,
					EntityManager& _entityManager,
					SystemManager& _systemManage
				);
			sf::RenderWindow& window;///< The main window
			TextureManager& textures;///< The texture manager
			FontManager& fonts;///< The font manager
			tgui::Gui& gui;///< The main GUI manager
			EventManager& eventManager;///< The event manager of the entity system.
			EntityManager& entityManager;///< The entity manager of the entity system
			SystemManager& systemManager;///< The system manager of the entity system
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
        /// \param dt Elapsed time since the last game frame.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt) = 0;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// The state must decide what to do with the event.
        /// The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) = 0;

	protected:
		void requestStackPush(States stateID);
		void requestStackPop();
		void requestStateClear();
		Context getContext() const;

	private:
		StateStack* m_stateStack;
		Context m_context;
};

#endif // STATE_H