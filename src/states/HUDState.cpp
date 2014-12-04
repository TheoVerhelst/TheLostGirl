#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState(StateStack& stack, Context context):
	State(stack, context),
	m_healthBar{nullptr},
	m_staminaBar{nullptr},
	m_windBar{nullptr}
{
	getContext().eventManager.subscribe<PlayerHealthChange>(*this);
	getContext().eventManager.subscribe<PlayerStaminaChange>(*this);
	
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
}

HUDState::~HUDState()
{
	getContext().gui.remove(m_healthBar);
	getContext().gui.remove(m_staminaBar);
	getContext().gui.remove(m_windBar);
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
	healthIsFading = false;
}

void HUDState::receive(const PlayerStaminaChange& playerStaminaChange)
{
	staminaIsFading = false;
}
