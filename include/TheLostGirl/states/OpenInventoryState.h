#ifndef OPENINVENTORYSTATE_H
#define OPENINVENTORYSTATE_H

#include <TGUI/TGUI.hpp>
#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>

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
struct ParametersChange;

/// State that effectively play the game.
/// The game become really interesting here.
class OpenInventoryState : public State
{
	public:
        /// Constructor.
        /// \param entity Entity of which open the inventory.
		OpenInventoryState(entityx::Entity entity);

		/// Destructor.
		~OpenInventoryState();

        /// The drawing function.
        /// \return virtual void
        /// It must do all things related to drawing stuff on the screen.
		virtual void draw() override;

        /// The logic update function.
        /// \param dt Elapsed time in the last game frame.
        /// \return Return true if the state under this one in the stack must be also updated.
        /// This function call e.g. the physic update function, the AI function, etc...
		virtual bool update(sf::Time dt) override;

        /// The event handling function.
        /// \param event Event to handle.
        /// \return Return true if the state under this state in the stack must be also updated.
        /// \note The closing window and resinzing window events are already handled by the Application class.
		virtual bool handleEvent(const sf::Event& event) override;

		/// Receive an event about a change in the parameters.
		/// \param parametersChange The data about the change.
		void receive(const ParametersChange& parametersChange);

	private:
		/// Reset all texts in the buttons and other widgets.
		void resetTexts();

		/// Fill the vertical layout with the items of m_entity.
		void fillContentDisplay();

		/// Function called by the scrollbar, used to scroll the grid display.
		/// \param newScrollValue the new value of the scrollbar.
		void scrollGrid(int newScrollValue);

		/// Function called by the scrollbar, used to scroll the list display.
		/// \param newScrollValue the new value of the scrollbar.
		void scrollList(int newScrollValue);

		/// Function called by the tab for switching the diplay mode.
		/// \param selectedTab The name of the new selected tab.
		void switchDisplay(sf::String selectedTab);

		/// Function called by the tab for switching the category mode.
		/// \param selectedTab The name of the new selected tab.
		void switchCategory(sf::String selectedTab);

		/// Checks if the two entities represents the same item.
		/// This is used to display similar items in a single line/square in the inventory display.
		/// \param left The item component of the item to compare.
		/// \param right The item component of the other entity to compare.
		/// \return True if the two entities represents the same item, false otherwhise.
		bool itemsAreEquals(const ItemComponent::Handle& left, const ItemComponent::Handle& right) const;

		entityx::Entity m_entity;                         ///< Entity of which open the inventory.
		tgui::ChildWindow::Ptr m_background;              ///< Background of the whole inventory window.
		tgui::Label::Ptr m_entityName;                    ///< Label with the name of the entity.
		tgui::Tab::Ptr m_displayTab;                      ///< Tab used for switching the display mode.
		std::vector<sf::String> m_displayStrings;         ///< The string used in display tabs, not translated.
		tgui::Tab::Ptr m_categoryTab;                     ///< Tab used for switching the shown category.
		std::vector<sf::String> m_categoryStrings;        ///< The string used in catgory tabs, not translated.

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
		std::list<ItemGridWidget> m_gridContent;          ///< List of all items to display.

		//Widget for the list display
		/// Hold GUI widgets to represent an item in the list inventory.
		struct ItemListWidget
		{
			~ItemListWidget();
			tgui::HorizontalLayout::Ptr layout;           ///< Layout for organizing the labels below.
			std::map<sf::String, tgui::Label::Ptr> labels;///< A label for each information about the item.
			std::list<entityx::Entity> items;             ///< The represented item.
		};
		tgui::Panel::Ptr m_listPanel;                     ///< Background for the whole list diplay.
		tgui::Scrollbar::Ptr m_listScrollbar;             ///< The scrollbar for the list display.
		std::list<ItemListWidget> m_listContent;          ///< All the widgets for the content of the list display.
		std::list<sf::String> m_columnStrings;            ///< The name of each kind of data in the list
		ItemListWidget m_listColumnTitles;                ///< Contains all title labels.
		tgui::VerticalLayout::Ptr m_listContentLayout;    ///< Main layout for the content of the list display.
		std::map<std::string, std::set<std::string>> m_categoriesPartition;///< Indicates the repartition of items categories between Ammo and Resources

};

#endif//OPENINVENTORYSTATE_H
