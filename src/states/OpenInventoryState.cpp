#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <entityx/entityx.h>
#include <TGUI/TGUI.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/Player.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/states/OpenInventoryState.hpp>

OpenInventoryState::OpenInventoryState(entityx::Entity entity) :
	m_entity(entity)
{
	Context::getEventManager().subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	using tgui::bindLeft;
	using tgui::bindTop;
	tgui::Gui& gui(Context::getGui());

	m_background = Context::getParameters().guiTheme->load("ChildWindow");
	m_background->setPosition(bindWidth(gui) * 0.25f, bindHeight(gui) * 0.125f);
	m_background->setSize(bindWidth(gui) * (0.5f/0.98f), bindHeight(gui) * 0.75f);//Larger background for scrolling bar
	m_background->connect("closed", [this]{requestStackPop(); Context::getPlayer().handleInitialInputState();});
	gui.add(m_background);

	m_entityName = Context::getParameters().guiTheme->load("Label");
	m_entityName->setTextSize(17);
	m_entityName->setPosition((bindWidth(m_background) - bindWidth(m_entityName)) * 0.5f, bindHeight(m_background) * 0.125f);
	m_background->add(m_entityName);

	m_displayStrings = {"List", "Grid"};
	m_displayTab = Context::getParameters().guiTheme->load("Tab");
	for(const sf::String& displayName : m_displayStrings)
		m_displayTab->add(displayName);
	m_displayTab->setPosition(bindWidth(m_background) - bindWidth(m_displayTab), bindHeight(m_background) * 0.23f);
	m_displayTab->setTabHeight(m_background->getSize().y * 0.07f);
	m_displayTab->select(m_displayStrings.front());
	m_displayTab->connect("tabselected", &OpenInventoryState::switchDisplay, this);
	m_background->add(m_displayTab);

	m_categoryStrings = {"All", "Ammo", "Resources"};
	m_categoryTab = Context::getParameters().guiTheme->load("Tab");
	for(const sf::String& categoryName : m_categoryStrings)
		m_categoryTab->add(categoryName);
	m_categoryTab->setPosition(0.f, bindHeight(m_background) * 0.23f);
	m_categoryTab->setTabHeight(m_background->getSize().y * 0.07f);
	m_categoryTab->select(m_categoryStrings.front());
	m_categoryTab->connect("tabselected", &OpenInventoryState::switchCategory, this);
	m_background->add(m_categoryTab);

	m_categoriesPartition["Ammo"] = {"Quiver", "Bow", "Arrow"};
	m_categoriesPartition["Resources"] = {"Skins"};

	//Make the grid
	m_gridPanel = std::make_shared<tgui::Panel>();
	m_gridPanel->setPosition(0.f, bindHeight(m_background) * 0.3f);
	m_gridPanel->setSize(bindWidth(m_background), bindHeight(m_background) * 0.8f);
	m_gridPanel->setBackgroundColor(sf::Color(255, 255, 255, 100));
	m_background->add(m_gridPanel);

    //Make the list
	m_listPanel = std::make_shared<tgui::Panel>();
	m_listPanel->setPosition(0.f, bindHeight(m_background) * 0.3f);
	m_listPanel->setSize(bindWidth(m_background), bindHeight(m_background) * 0.8f);
	m_listPanel->setBackgroundColor(sf::Color(255, 255, 255, 100));
	m_background->add(m_listPanel);

	m_columnStrings = {"Qtty", "Name", "Category", "Weight", "Value", "Value/Weight"};
	m_listColumnTitles = ItemListWidget();
	m_listColumnTitles.layout = std::make_shared<tgui::HorizontalLayout>();
	m_listColumnTitles.layout->setPosition(0.f, 5.f);
	m_listColumnTitles.layout->setSize(bindWidth(m_listPanel), 25.f);
	m_listColumnTitles.layout->addSpace(0.1f);
	m_listPanel->add(m_listColumnTitles.layout);
	for(const sf::String& columnName : m_columnStrings)
	{
		tgui::Label::Ptr label = Context::getParameters().guiTheme->load("Label");
		label->setText(columnName);
		label->setTextSize(20);
		m_listColumnTitles.layout->add(label);
		m_listColumnTitles.labels.emplace(columnName, label);
	}

	m_listContentLayout = std::make_shared<tgui::VerticalLayout>();
	m_listContentLayout->setPosition(0.f, 30.f);
	m_listContentLayout->setSize(bindWidth(m_listPanel) * 0.98f, 15*m_entity.component<InventoryComponent>()->items.size());
    m_listPanel->add(m_listContentLayout);

	//Set scrollbars
	m_listScrollbar = Context::getParameters().guiTheme->load("Scrollbar");
	m_listScrollbar->setPosition(bindWidth(m_listPanel) * 0.98f, 0.f);
	m_listScrollbar->setSize(bindWidth(m_listPanel) * 0.02f, bindHeight(m_listPanel));
    m_listScrollbar->setArrowScrollAmount(30);
    m_listScrollbar->setLowValue(int(m_gridPanel->getSize().y));
    m_listScrollbar->setMaximum(int(m_listContentLayout->getSize().y));
    m_listScrollbar->connect("valuechanged", &OpenInventoryState::scrollList, this);
    m_listPanel->add(m_listScrollbar);
	m_listPanel->hide();

	m_gridScrollbar = Context::getParameters().guiTheme->load("Scrollbar");
	m_gridScrollbar->setPosition(bindWidth(m_gridPanel) * 0.98f, 0.f);
	m_gridScrollbar->setSize(bindWidth(m_gridPanel) * 0.02f, bindHeight(m_gridPanel));
    m_gridScrollbar->setArrowScrollAmount(30);
    m_gridScrollbar->connect("valuechanged", &OpenInventoryState::scrollGrid, this);
    m_gridPanel->add(m_gridScrollbar);

	fillContentDisplay();
	resetTexts();
}

OpenInventoryState::~OpenInventoryState()
{
	tgui::Gui& gui(Context::getGui());
	gui.remove(m_background);
}

void OpenInventoryState::draw()
{}

bool OpenInventoryState::update(sf::Time)
{
	return false;
}

bool OpenInventoryState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
	{
		requestStackPop();
		Context::getPlayer().handleInitialInputState();
	}
	return false;
}

void OpenInventoryState::receive(const ParametersChange& parametersChange)
{
	if(parametersChange.langChanged)
		resetTexts();
}

void OpenInventoryState::resetTexts()
{
	if(m_entityName)
	{
		const auto nameComponent(m_entity.component<NameComponent>());
		if(nameComponent)
			m_entityName->setText(Context::getLangManager().tr(nameComponent->name));
		else
			m_entityName->setText("");
	}

	for(std::size_t i{0}; i < m_displayTab->getTabsCount(); ++i)
		m_displayTab->changeText(i, Context::getLangManager().tr(m_displayStrings[i]));

	for(std::size_t i{0}; i < m_categoryTab->getTabsCount(); ++i)
		m_categoryTab->changeText(i, Context::getLangManager().tr(m_categoryStrings[i]));

	for(ItemGridWidget& itemWidget : m_gridContent)
		if(itemWidget.caption)
			itemWidget.caption->setText(Context::getLangManager().tr(itemWidget.item.component<ItemComponent>()->type));

	for(auto& labelPair : m_listColumnTitles.labels)
		labelPair.second->setText(Context::getLangManager().tr(labelPair.first));

	for(ItemListWidget& itemWidget : m_listContent)
	{
		const ItemComponent::Handle itemComponent(itemWidget.items.front().component<ItemComponent>());
		auto it(itemWidget.labels.find("Qtty"));
		if(it != itemWidget.labels.end())
			it->second->setText(std::to_wstring(itemWidget.items.size()));
		it = itemWidget.labels.find("Category");
		if(it != itemWidget.labels.end())
			it->second->setText(Context::getLangManager().tr(itemComponent->category));
		it = itemWidget.labels.find("Name");
		if(it != itemWidget.labels.end())
			it->second->setText(Context::getLangManager().tr(itemComponent->type));
		it = itemWidget.labels.find("Weight");
		if(it != itemWidget.labels.end())
			it->second->setText(std::to_wstring(itemComponent->weight));
		it = itemWidget.labels.find("Value");
		if(it != itemWidget.labels.end())
			it->second->setText(std::to_wstring(itemComponent->value));
		it = itemWidget.labels.find("Value/Weight");
		if(it != itemWidget.labels.end())
			it->second->setText(std::to_wstring(itemComponent->value/itemComponent->weight));
	}
}

void OpenInventoryState::fillContentDisplay()
{
	const InventoryComponent::Handle inventoryComponent{m_entity.component<InventoryComponent>()};
	if(not TEST(inventoryComponent))
		return;

	//Clear the grid and the list
	for(auto& itemWidget : m_listContent)
		m_listContentLayout->remove(itemWidget.layout);
	m_listContent.clear();
	for(auto& itemWidget : m_gridContent)
		m_gridPanel->remove(itemWidget.background);
	m_gridContent.clear();

	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	const float itemSize{120.f};
	for(auto& entityItem : m_entity.component<InventoryComponent>()->items)
	{
		ItemComponent::Handle itemComponent(entityItem.component<ItemComponent>());
		if(not TEST(itemComponent))
			continue;

		const std::string type{entityItem.component<ItemComponent>()->type};
		const std::string category{entityItem.component<ItemComponent>()->category};

		//Test if the current item whould be displayed according to the selected tab
		const std::string& currentTab{m_categoryTab->getSelected()};
		auto subCategories = m_categoriesPartition.find(currentTab);
		if(subCategories != m_categoriesPartition.end())
			if(subCategories->second.find(itemComponent->category) == subCategories->second.end())
				continue;

		//Fill the list
		bool foundSimilarItem{false};
		for(auto& itemWidget : m_listContent)
		{
			if(itemsAreEquals(itemComponent, itemWidget.items.front().component<ItemComponent>()))
			{
				itemWidget.items.push_back(entityItem);
				foundSimilarItem = true;
				break;
			}
		}
		if(not foundSimilarItem)
		{
			ItemListWidget itemWidget;
			itemWidget.items.push_back(entityItem);
			itemWidget.layout = std::make_shared<tgui::HorizontalLayout>();
			itemWidget.layout->addSpace(0.1f);
			for(auto& columnStr : m_columnStrings)
			{
				tgui::Label::Ptr label = Context::getParameters().guiTheme->load("Label");
				label->setTextSize(8);
				itemWidget.layout->add(label);
				itemWidget.labels.emplace(columnStr, label);
			}
			m_listContent.push_back(itemWidget);
			m_listContentLayout->add(itemWidget.layout);
		}

		//Fill the grid
		ItemGridWidget itemWidget;
		itemWidget.background = std::make_shared<tgui::Panel>();
		itemWidget.background->setBackgroundColor(sf::Color::Transparent);
		itemWidget.background->setSize(itemSize, itemSize);
		itemWidget.background->setPosition(itemSize*columnCounter, itemSize*rowCounter);

		itemWidget.picture = std::make_shared<tgui::Picture>(Context::getParameters().resourcesPath + "images/items/" + category + "/" + type + ".png");
		itemWidget.picture->setPosition(itemSize/6.f, 0.f);
		itemWidget.background->add(itemWidget.picture);

		itemWidget.caption = Context::getParameters().guiTheme->load("Label");
		itemWidget.caption->setPosition((bindWidth(itemWidget.background) * 0.5f) - (bindWidth(itemWidget.caption) * 0.5f), itemSize/1.2f);
		itemWidget.caption->setTextSize(8);
		itemWidget.background->add(itemWidget.caption);

		itemWidget.item = entityItem;
		m_gridPanel->add(itemWidget.background);
		m_gridContent.push_back(itemWidget);
		rowCounter = (++itemCounter) / 8;
		columnCounter = itemCounter % 8;
	}
	//Set rowCounter to the total number of rows
	rowCounter = ((--itemCounter) / 8)+1;
    m_gridScrollbar->setLowValue(int(m_gridPanel->getSize().y));
    //Set the maximum value to 120*number of rows
    m_gridScrollbar->setMaximum(int(itemSize*float(rowCounter)));
}

void OpenInventoryState::scrollGrid(int newScrollValue)
{
	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	const float itemSize{120.f};
	for(auto& itemWidget : m_gridContent)
	{
		itemWidget.background->setPosition(itemSize*columnCounter, itemSize*rowCounter-newScrollValue);
		rowCounter = (++itemCounter) / 8;
		columnCounter = itemCounter % 8;
	}
}

void OpenInventoryState::scrollList(int newScrollValue)
{
	m_listContentLayout->setPosition(m_listContentLayout->getPosition().x, 30.f-newScrollValue);
}

void OpenInventoryState::switchDisplay(sf::String selectedTab)
{
	if(selectedTab == "Grid")
	{
		m_listPanel->hide();
		m_gridPanel->show();
	}
	else if(selectedTab == "List")
	{
		m_gridPanel->hide();
		m_listPanel->show();
	}
}

void OpenInventoryState::switchCategory(sf::String)
{
	fillContentDisplay();
	resetTexts();
}

bool OpenInventoryState::itemsAreEquals(const ItemComponent::Handle& left, const ItemComponent::Handle& right) const
{
	return left->type == right->type and left->category == right->category and left->weight - right->weight < 0.0001f and left->value - right->value < 0.0001f;
}

OpenInventoryState::ItemListWidget::~ItemListWidget()
{
}
