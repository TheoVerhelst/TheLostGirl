#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState(StateStack& stack, Context context):
	State(stack, context),
	m_healthLabel{nullptr},
	m_staminaLabel{nullptr}
{
	getContext().eventManager.subscribe<PlayerHealthChange>(*this);
	getContext().eventManager.subscribe<PlayerStaminaChange>(*this);
	
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui = getContext().gui;
	
	m_healthLabel = tgui::Label::create();
	m_healthLabel->setTextSize(20);
	m_healthLabel->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.1f));
	m_healthLabel->setTextColor(sf::Color::Black);
	m_healthLabel->setText("0");
	gui.add(m_healthLabel);
	
	m_staminaLabel = tgui::Label::create();
	m_staminaLabel->setTextSize(20);
	m_staminaLabel->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.15f));
	m_staminaLabel->setTextColor(sf::Color::Black);
	m_staminaLabel->setText("0");
	gui.add(m_staminaLabel);
}

HUDState::~HUDState()
{
	getContext().gui.remove(m_healthLabel);
	getContext().gui.remove(m_staminaLabel);
}

void HUDState::draw()
{}

bool HUDState::update(sf::Time)
{
	return true;
}

bool HUDState::handleEvent(const sf::Event&)
{
	return true;
}

void HUDState::receive(const PlayerHealthChange& playerHealthChange)
{
	std::string healthStr = std::to_string(playerHealthChange.newHealth);
	m_healthLabel->setText(healthStr.substr(0, healthStr.find(".")));
}

void HUDState::receive(const PlayerStaminaChange& playerStaminaChange)
{
	std::string staminaStr = std::to_string(playerStaminaChange.newStamina);
	m_staminaLabel->setText(staminaStr.substr(0, staminaStr.find(".")));
}