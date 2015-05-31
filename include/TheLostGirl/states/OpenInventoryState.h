#ifndef OPENINVENTORYSTATE_H
#define OPENINVENTORYSTATE_H

#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <TGUI/Panel.hpp>
#include <TGUI/Canvas.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/ChildWindow.hpp>
#include <TGUI/Picture.hpp>
#include <TGUI/Tab.hpp>
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
struct ParametersChange;

/// State that effectively play the game.
/// The game become really interesting here.
class OpenInventoryState : public State
{
	public:
        /// Constructor.
        /// \param stack StateStack wherein the State is added.
        /// \param entity Entity of which open the inventory.
		OpenInventoryState(StateStack& stack, entityx::Entity entity);

		/// Destructor.
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

		/// Receive an event about a change in the parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

	private:
		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		void scrollGrid(int newScrollValue);

		void scrollList(int newScrollValue);

		void switchDisplay(sf::String selectedTab);

		/// Hold GUI widgets to represent an item in the grid inventory.
		struct ItemGridWidget
		{
			/// Destructor.
			~ItemGridWidget()
			{}

			tgui::Panel::Ptr background;///< The background of the item.
			tgui::Picture::Ptr picture; ///< The image of the item.
			tgui::Label::Ptr caption;   ///< A label with the naim of the item.
			entityx::Entity item;       ///< The represented item.
		};
		entityx::Entity m_entity;               ///< Entity of which open the inventory.
		tgui::ChildWindow::Ptr m_background;    ///< Background of the whole inventory window.
		tgui::Label::Ptr m_entityName;          ///< Label with the name of the entity.
		tgui::Tab::Ptr m_displayTab;
		tgui::Tab::Ptr m_categoryTab;

		tgui::Panel::Ptr m_gridPanel;
		tgui::Scrollbar::Ptr m_gridScrollbar;
		std::list<ItemGridWidget> m_itemWidgets;///< List of all items to display.

		/// Hold GUI widgets to represent an item in the list inventory.
		struct ItemListWidget
		{
			std::map<sf::String, tgui::Label::Ptr> labels;
			entityx::Entity item;                         ///< The represented item.
		};

		tgui::Panel::Ptr m_listPanel;
		std::list<sf::String> m_columns;
		std::map<sf::String, tgui::Label::Ptr> m_columnsTitles;
		std::list<ItemListWidget> m_columnsContent;
		tgui::Scrollbar::Ptr m_listScrollbar;
		tgui::Grid::Ptr m_listGrid;

};

#endif//OPENINVENTORYSTATE_H
