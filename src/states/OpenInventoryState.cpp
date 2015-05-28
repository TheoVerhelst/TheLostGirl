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
	m_entity(entity),
	m_background{nullptr}
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

	m_gridPanel = tgui::Panel::create();
	m_gridPanel->setPosition(0.f, bindHeight(m_background, 0.2f));
	m_gridPanel->setSize(bindWidth(m_background), bindHeight(m_background, 0.8f));
	m_gridPanel->setBackgroundColor(sf::Color::Transparent);
	m_background->add(m_gridPanel);

	m_gridScrollbar = tgui::Scrollbar::create(getContext().parameters.guiConfigFile);
	m_gridScrollbar->setPosition(bindWidth(m_gridPanel, 0.98f), 0.f);
	m_gridScrollbar->setSize(bindWidth(m_gridPanel, 0.02f), bindHeight(m_gridPanel));
    m_gridScrollbar->setArrowScrollAmount(30);
    m_gridScrollbar->connect("valuechanged", &OpenInventoryState::scrollGrid, this);
    m_gridPanel->add(m_gridScrollbar);

	m_grid = tgui::Grid::create();
	m_grid->setSize(bindWidth(m_gridPanel, 0.98f), bindHeight(m_gridPanel));
	m_gridPanel->add(m_grid);
	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	for(auto& entityItem : m_entity.component<InventoryComponent>()->items)
	{
		std::string type{entityItem.component<ItemComponent>()->type};
		std::string category{entityItem.component<ItemComponent>()->category};
		ItemGridWidget itemWidget;
		itemWidget.background = tgui::Panel::create();
		itemWidget.background->setBackgroundColor(sf::Color(255, 255, 255, 100));
		itemWidget.background->setSize(120*getContext().parameters.scale, 120*getContext().parameters.scale);
		itemWidget.picture = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "items/" + category + "/" + type + ".png");
		itemWidget.background->add(itemWidget.picture);
		itemWidget.picture->move(bindWidth(itemWidget.background, 0.08333333f), bindHeight(itemWidget.background, 0.08333333f));
		itemWidget.caption = tgui::Label::create(getContext().parameters.guiConfigFile);
		itemWidget.background->add(itemWidget.caption);
		itemWidget.item = entityItem;
		m_grid->addWidget(itemWidget.background, rowCounter, columnCounter);
		m_itemWidgets.push_back(itemWidget);
		rowCounter = (++itemCounter) / 8;
		columnCounter = itemCounter % 8;
	}
	//Set rowCounter to the total number of rows
	rowCounter = ((--itemCounter) / 8)+1;
    m_gridScrollbar->setLowValue(int(m_gridPanel->getSize().y));
    //Set the maximum value to 120*scale*number of rows
    m_gridScrollbar->setMaximum(int(120.f*getContext().parameters.scale*float(rowCounter)));
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
		if(m_entity.has_component<NameComponent>())
			m_entityName->setText(getContext().langManager.tr(m_entity.component<NameComponent>()->name));
		else
			m_entityName->setText("");
	}
	for(ItemGridWidget& itemWidget : m_itemWidgets)
		if(itemWidget.caption)
			itemWidget.caption->setText(getContext().langManager.tr(itemWidget.item.component<ItemComponent>()->type));
}

void OpenInventoryState::scrollGrid(int newScrollValue)
{
	if(m_grid)
		m_grid->setPosition(m_grid->getPosition().x, -newScrollValue);
}
