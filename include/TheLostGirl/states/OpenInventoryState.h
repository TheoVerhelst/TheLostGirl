#ifndef OPENINVENTORYSTATE_H
#define OPENINVENTORYSTATE_H

#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <TGUI/Panel.hpp>
#include <TGUI/Picture.hpp>
#include <TGUI/Label.hpp>
#include <TGUI/Grid.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <TheLostGirl/AnimationsManager.h>
#include <TheLostGirl/ContactListener.h>
#include <TheLostGirl/State.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
namespace entityx
{
	class Entity;
}
class StateStack;
struct Transform;

/// State that effectively play the game.
/// The game become really interesting here.
class OpenInventoryState : public State
{
	public:
        /// Default constructor.
        /// \param stack StateStack wherein the State is added.
		OpenInventoryState(StateStack& stack, entityx::Entity entity);

		/// Default destructor.
		/// Remove all bodies, sprites and others from the memory.
		~OpenInventoryState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw();

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt);

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event);

	private:
		struct ItemWidget
		{
			tgui::Panel::Ptr background;
			tgui::Picture::Ptr picture;
			tgui::Label::Ptr caption;
		};
		entityx::Entity m_entity;
		tgui::Panel::Ptr m_background;
		tgui::Label::Ptr m_entityName;
		tgui::Grid::Ptr m_grid;
		std::list<ItemWidget> m_itemWidgets;
};

#endif//OPENINVENTORYSTATE_H
