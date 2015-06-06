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
#include <TGUI/HorizontalLayout.hpp>
#include <TGUI/VerticalLayout.hpp>
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

		/// Function called by the scrollbar, used to scroll the grid display.
		/// \param newScrollValue the new value of the scrollbar.
		void scrollGrid(int newScrollValue);

		/// Function called by the scrollbar, used to scroll the list display.
		/// \param newScrollValue the new value of the scrollbar.
		void scrollList(int newScrollValue);

		/// Function called by the tab for switching the diplay mode.
		/// \param selectedTab The name of the new selected tab.
		void switchDisplay(sf::String selectedTab);

		entityx::Entity m_entity;                         ///< Entity of which open the inventory.
		tgui::ChildWindow::Ptr m_background;              ///< Background of the whole inventory window.
		tgui::Label::Ptr m_entityName;                    ///< Label with the name of the entity.
		tgui::Tab::Ptr m_displayTab;                      ///< Tab used for switching the display mode.
		tgui::Tab::Ptr m_categoryTab;                     ///< Tab used for switching the shown category.

		//Widgets for the grid display
		/// Hold GUI widgets to represent an item in the grid inventory.
		struct ItemGridWidget
		{
			tgui::Panel::Ptr background;                  ///< The background of the item.
			tgui::Picture::Ptr picture;                   ///< The image of the item.
			tgui::Label::Ptr caption;                     ///< A label with the naim of the item.
			entityx::Entity item;                         ///< The represented item.
		};
		tgui::Panel::Ptr m_gridPanel;                     ///< Background of the whole grid display.
		tgui::Scrollbar::Ptr m_gridScrollbar;             ///< Scrollbar for scrolling the grid.
		std::list<ItemGridWidget> m_itemWidgets;          ///< List of all items to display.

		//Widget for the list display
		/// Hold GUI widgets to represent an item in the list inventory.
		struct ItemListWidget
		{
			tgui::HorizontalLayout::Ptr layout;           ///< Layout for organizing the labels below.
			std::map<sf::String, tgui::Label::Ptr> labels;///< A label for each information about the item.
			entityx::Entity item;                         ///< The represented item.
		};
		tgui::Panel::Ptr m_listPanel;                     ///< Background for the whole list diplay.
		std::list<sf::String> m_listColumnsNames;         ///< The name of each kind of data in the list
		ItemListWidget m_listColumnTitles;                ///< Contains all title labels.
		tgui::VerticalLayout::Ptr m_listContentLayout;    ///< Main layout for the content of the list display.
		std::list<ItemListWidget> m_listContent;          ///< All the widgets for the content of the list display.
		tgui::Scrollbar::Ptr m_listScrollbar;             ///< The scrollbar for the list display.

};

#endif//OPENINVENTORYSTATE_H
