#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/components.h>

#include <TheLostGirl/states/OpenInventoryState.h>

OpenInventoryState::OpenInventoryState(StateStack& stack, entityx::Entity entity) :
	State(stack),
	m_entity(entity),
	m_background{nullptr}
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	using tgui::bindLeft;
	using tgui::bindTop;
	tgui::Gui& gui(getContext().gui);

	m_background = tgui::Panel::create();
	m_background->setPosition(bindWidth(gui, 0.25f), bindHeight(gui, 0.125f));
	m_background->setSize(bindWidth(gui, 0.5f), bindHeight(gui, 0.75f));
	m_background->setBackgroundColor(sf::Color(255, 255, 255, 100));
	gui.add(m_background);

	m_entityName = tgui::Label::create();
	if(entity.has_component<NameComponent>())
		m_entityName->setText(entity.component<NameComponent>()->name);
	else
		m_entityName->setText("");
	m_entityName->setTextColor(sf::Color::Black);
	m_entityName->setTextSize(35);
	m_entityName->setPosition(bindWidth(m_background, 0.5f) - bindWidth(m_entityName, 0.5f), bindHeight(m_background, 0.125f));
	m_background->add(m_entityName);

	m_grid = tgui::Grid::create();
	m_grid->setPosition(0.f, bindHeight(m_background, 0.25f));
	m_grid->setSize(bindWidth(m_background), bindHeight(m_background, 0.5f));
	m_background->add(m_grid);
	unsigned int rowCounter{0}, columnCounter{0}, itemCounter{0};
	for(auto& entityItem : m_entity.component<InventoryComponent>()->items)
	{
		std::string type{entityItem.component<ItemComponent>()->type};
		std::string category{entityItem.component<ItemComponent>()->category};
		ItemWidget itemWidget;
		itemWidget.background = tgui::Panel::create();
		itemWidget.background->setBackgroundColor(sf::Color(255, 255, 255, 100));
		itemWidget.background->setSize(120*getContext().parameters.scale, 120*getContext().parameters.scale);
		itemWidget.picture = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "items/" + category + "/" + type + ".png");
		itemWidget.background->add(itemWidget.picture);
		itemWidget.picture->move(bindWidth(itemWidget.background, 0.08333333f), bindHeight(itemWidget.background, 0.08333333f));
		itemWidget.caption = tgui::Label::create();
		itemWidget.caption->setText(type);
		itemWidget.caption->setTextColor(sf::Color::Black);
		itemWidget.background->add(itemWidget.caption);
		m_grid->addWidget(itemWidget.background, rowCounter, columnCounter);
		m_itemWidgets.push_back(itemWidget);
		rowCounter = (++itemCounter) / 8;
		columnCounter = itemCounter % 8;
	}
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

OpenInventoryState::ItemWidget::~ItemWidget()
{}
