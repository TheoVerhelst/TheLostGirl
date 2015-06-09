#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <TGUI/Gui.hpp>
#include <TheLostGirl/State.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/states/OpenInventoryState.h>

OpenInventoryState::OpenInventoryState(StateStack& stack, entityx::Entity entity) :
	State(stack),
	m_entity(entity)
{
	getContext().eventManager.subscribe<ParametersChange>(*this);
	using tgui::bindWidth;
	using tgui::bindHeight;
	using tgui::bindLeft;
	using tgui::bindTop;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::ChildWindow::create(getContext().parameters.guiConfigFile);
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.125f));
	m_background->setSize(bindWidth(gui, 0.5f/0.98f), bindHeight(gui, 0.75f));//Larger background for scrolling bar
	m_background->connect("closed", [this]{requestStackPop(); getContext().player.handleInitialInputState();});
	gui.add(m_background);

	m_entityName = tgui::Label::create(getContext().parameters.guiConfigFile);
	m_entityName->setTextSize(35);
	m_entityName->setPosition(bindWidth(m_background, 0.5f) - bindWidth(m_entityName, 0.5f), bindHeight(m_background, 0.125f));
	m_background->add(m_entityName);

	m_displayTab = tgui::Tab::create(getContext().parameters.guiConfigFile);
	m_displayTab->add("List");
	m_displayTab->add("Grid");
	m_displayTab->setPosition(bindWidth(m_background) - bindWidth(m_displayTab), bindHeight(m_background, 0.23f));
	m_displayTab->setTabHeight(m_background->getSize().y*0.07f);
	m_displayTab->connect("tabselected", &OpenInventoryState::switchDisplay, this);
	m_background->add(m_displayTab);

	m_categoryTab = tgui::Tab::create(getContext().parameters.guiConfigFile);
	m_categoryTab->add("All");
	m_categoryTab->add("Ammo");
	m_categoryTab->add("Resources");
	m_categoryTab->setPosition(0.f, bindHeight(m_background, 0.23f));
	m_categoryTab->setTabHeight(m_background->getSize().y*0.07f);
	m_background->add(m_categoryTab);

	//Make the grid
	m_gridPanel = tgui::Panel::create();
	m_gridPanel->setPosition(0.f, bindHeight(m_background, 0.3f));
	m_gridPanel->setSize(bindWidth(m_background), bindHeight(m_background, 0.8f));
	m_gridPanel->setBackgroundColor(sf::Color(255, 255, 255, 100));
	m_background->add(m_gridPanel);

	m_gridScrollbar = tgui::Scrollbar::create(getContext().parameters.guiConfigFile);
	m_gridScrollbar->setPosition(bindWidth(m_gridPanel, 0.98f), 0.f);
	m_gridScrollbar->setSize(bindWidth(m_gridPanel, 0.02f), bindHeight(m_gridPanel));
    m_gridScrollbar->setArrowScrollAmount(30);
    m_gridScrollbar->connect("valuechanged", &OpenInventoryState::scrollGrid, this);
    m_gridPanel->add(m_gridScrollbar);

	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	const float itemSize{120.f*getContext().parameters.scale};
	for(auto& entityItem : m_entity.component<InventoryComponent>()->items)
	{
		std::string type{entityItem.component<ItemComponent>()->type};
		std::string category{entityItem.component<ItemComponent>()->category};
		ItemGridWidget itemWidget;
		itemWidget.background = tgui::Panel::create();
		itemWidget.background->setBackgroundColor(sf::Color::Transparent);
		itemWidget.background->setSize(itemSize, itemSize);
		itemWidget.background->setPosition(itemSize*columnCounter, itemSize*rowCounter);

		itemWidget.picture = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "items/" + category + "/" + type + ".png");
		itemWidget.picture->setPosition(itemSize/6.f, 0.f);
		itemWidget.background->add(itemWidget.picture);

		itemWidget.caption = tgui::Label::create(getContext().parameters.guiConfigFile);
		itemWidget.caption->setPosition(bindWidth(itemWidget.background, 0.5f)-bindWidth(itemWidget.caption, 0.5f), itemSize/1.2f);
		itemWidget.caption->setTextSize(15.f);
		itemWidget.background->add(itemWidget.caption);

		itemWidget.item = entityItem;
		m_gridPanel->add(itemWidget.background);
		m_itemWidgets.push_back(itemWidget);
		rowCounter = (++itemCounter) / 8;
		columnCounter = itemCounter % 8;
	}
	//Set rowCounter to the total number of rows
	rowCounter = ((--itemCounter) / 8)+1;
    m_gridScrollbar->setLowValue(int(m_gridPanel->getSize().y));
    //Set the maximum value to 120*scale*number of rows
    m_gridScrollbar->setMaximum(int(itemSize*float(rowCounter)));

    //Make the list
	m_listPanel = tgui::Panel::create();
	m_listPanel->setPosition(0.f, bindHeight(m_background, 0.3f));
	m_listPanel->setSize(bindWidth(m_background), bindHeight(m_background, 0.8f));
	m_listPanel->setBackgroundColor(sf::Color(255, 255, 255, 100));
	m_background->add(m_listPanel);

	m_listColumnsNames.push_back("Qtty");
	m_listColumnsNames.push_back("Name");
	m_listColumnsNames.push_back("Category");
	m_listColumnsNames.push_back("Weight");
	m_listColumnsNames.push_back("Value");
	m_listColumnsNames.push_back("Value/Weight");

	m_listColumnTitles = ItemListWidget();
	m_listColumnTitles.layout = tgui::HorizontalLayout::create();
	m_listColumnTitles.layout->setPosition(0.f, 5.f);
	m_listColumnTitles.layout->setSize(bindWidth(m_listPanel), 25.f);
	m_listPanel->add(m_listColumnTitles.layout);
	for(auto& column : m_listColumnsNames)
	{
		auto label = tgui::Label::create(getContext().parameters.guiConfigFile);
		label->setText(column);
		label->setTextSize(20);
		m_listColumnTitles.layout->add(label);
		m_listColumnTitles.labels.emplace(column, label);
	}

	m_listContentLayout = tgui::VerticalLayout::create();
	m_listContentLayout->setPosition(0.f, 30.f);
	m_listContentLayout->setSize(bindWidth(m_listPanel, 0.98f), 15*m_entity.component<InventoryComponent>()->items.size());
    m_listPanel->add(m_listContentLayout);

	//Fill the vertical content layout
	for(auto& entityItem : m_entity.component<InventoryComponent>()->items)
	{
		ItemListWidget itemWidget;
		itemWidget.item = entityItem;
		itemWidget.layout = tgui::HorizontalLayout::create();
		for(auto& columnStr : m_listColumnsNames)
		{
			tgui::Label::Ptr label = tgui::Label::create(getContext().parameters.guiConfigFile);
			label->setTextSize(15);
			itemWidget.layout->add(label);
			itemWidget.labels.emplace(columnStr, label);
		}
		m_listContent.push_back(itemWidget);
		m_listContentLayout->add(itemWidget.layout);
	}

	m_listScrollbar = tgui::Scrollbar::create(getContext().parameters.guiConfigFile);
	m_listScrollbar->setPosition(bindWidth(m_listPanel, 0.98f), 0.f);
	m_listScrollbar->setSize(bindWidth(m_listPanel, 0.02f), bindHeight(m_listPanel));
    m_listScrollbar->setArrowScrollAmount(30);
    m_listScrollbar->setLowValue(int(m_gridPanel->getSize().y));
    m_listScrollbar->setMaximum(int(m_listContentLayout->getSize().y));
    m_listScrollbar->connect("valuechanged", &OpenInventoryState::scrollList, this);
    m_listPanel->add(m_listScrollbar);
	m_listPanel->hide();

	resetTexts();
}

OpenInventoryState::~OpenInventoryState()
{
	tgui::Gui& gui(getContext().gui);
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
		getContext().player.handleInitialInputState();
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
			m_entityName->setText(getContext().langManager.tr(nameComponent->name));
		else
			m_entityName->setText("");
	}

	for(ItemGridWidget& itemWidget : m_itemWidgets)
		if(itemWidget.caption)
			itemWidget.caption->setText(getContext().langManager.tr(itemWidget.item.component<ItemComponent>()->type));

	for(auto& labelPair : m_listColumnTitles.labels)
		labelPair.second->setText(getContext().langManager.tr(labelPair.first));

	for(ItemListWidget& itemWidget : m_listContent)
	{
		auto nameIt(itemWidget.labels.find("Name"));
		if(nameIt != itemWidget.labels.end())
			nameIt->second->setText(getContext().langManager.tr(itemWidget.item.component<ItemComponent>()->type));
		auto cetegoryIt(itemWidget.labels.find("Category"));
		if(cetegoryIt != itemWidget.labels.end())
			cetegoryIt->second->setText(getContext().langManager.tr(itemWidget.item.component<ItemComponent>()->category));
	}
}

void OpenInventoryState::scrollGrid(int newScrollValue)
{
	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	const float itemSize{120.f*getContext().parameters.scale};
	for(auto& itemWidget : m_itemWidgets)
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
	m_gridPanel->hide();
	m_listPanel->hide();
	if(selectedTab == "Grid")
		m_gridPanel->show();
	else if(selectedTab == "List")
		m_listPanel->show();
}
