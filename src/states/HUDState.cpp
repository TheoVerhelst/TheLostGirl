#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/System.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/systems/TimeSystem.h>

#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState(StateStack& stack):
	State(stack),
	m_healthBar{nullptr},
	m_staminaBar{nullptr},
	m_windBar{nullptr},
	m_healthSpr{},
	m_healthBorderSpr{},
	m_staminaSpr{},
	m_staminaBorderSpr{},
	m_windStrengthSpr{},
	m_windStrengthBarSpr{},
	m_healthIsFading{true},
	m_staminaIsFading{true},
	m_windIsFading{true}
{
	getContext().eventManager.subscribe<PlayerHealthChange>(*this);
	getContext().eventManager.subscribe<PlayerStaminaChange>(*this);

	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	float scale{getContext().parameters.scale};
	TextureManager& texManager(getContext().textureManager);

	//Load textures
	texManager.load("health ath", paths[getContext().parameters.scaleIndex] + "healthAth.png");
	texManager.load("health border ath", paths[getContext().parameters.scaleIndex] + "healthBorderAth.png");
	texManager.load("stamina ath", paths[getContext().parameters.scaleIndex] + "staminaAth.png");
	texManager.load("stamina border ath", paths[getContext().parameters.scaleIndex] + "staminaBorderAth.png");
	texManager.load("wind arrow ath", paths[getContext().parameters.scaleIndex] + "windArrowAth.png");
	texManager.load("wind bar ath", paths[getContext().parameters.scaleIndex] + "windBarAth.png");

	//Set textures to sprites
	m_healthSpr.setTexture(texManager.get("health ath"));
	m_healthSpr.setTextureRect({0, 0, scaleRes(240, scale), scaleRes(20, scale)});
	m_healthBorderSpr.setTexture(texManager.get("health border ath"));
	m_staminaSpr.setTexture(texManager.get("stamina ath"));
	m_staminaSpr.setTextureRect({0, 0, scaleRes(240, scale), scaleRes(20, scale)});
	m_staminaBorderSpr.setTexture(texManager.get("stamina border ath"));
	m_windStrengthSpr.setTexture(texManager.get("wind arrow ath"));
	m_windStrengthSpr.setPosition(120.f*scale, 0);
	m_windStrengthBarSpr.setTexture(texManager.get("wind bar ath"));

	m_healthBar = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
	m_healthBar->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.99f) - bindHeight(m_healthBar));
	gui.add(m_healthBar);
	m_staminaBar = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
	m_staminaBar->setPosition(bindWidth(gui, 0.99f) - bindWidth(m_staminaBar), bindHeight(gui, 0.99f) - bindHeight(m_staminaBar));
	gui.add(m_staminaBar);
	m_windBar = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
	m_windBar->setPosition(bindWidth(gui, 0.99f) - bindWidth(m_windBar), bindHeight(gui, 0.99f) - bindHeight(m_windBar) - bindHeight(m_staminaBar, 1.5));
	gui.add(m_windBar);
}

HUDState::~HUDState()
{
	getContext().gui.remove(m_healthBar);
	getContext().gui.remove(m_staminaBar);
	getContext().gui.remove(m_windBar);
}

void HUDState::draw()
{
	m_healthBar->clear(sf::Color::Transparent);
	m_healthBar->draw(m_healthSpr);
	m_healthBar->draw(m_healthBorderSpr);
	m_healthBar->display();
	m_staminaBar->clear(sf::Color::Transparent);
	m_staminaBar->draw(m_staminaSpr);
	m_staminaBar->draw(m_staminaBorderSpr);
	m_staminaBar->display();
	m_windBar->clear(sf::Color::Transparent);
	m_windBar->draw(m_windStrengthBarSpr);
	m_windBar->draw(m_windStrengthSpr);
	m_windBar->display();
}

bool HUDState::update(sf::Time)
{
	//Get the wind strength and compute the wind ath
	float scale{getContext().parameters.scale};
	float windStrength{cap(getContext().systemManager.system<TimeSystem>()->getWindStrength()/5.f, -1, 1)};

	bool oldFadingState{m_windIsFading};
	m_windIsFading = std::abs(windStrength) < 0.125f;
	if(m_windIsFading)
	{
		//If the fading start now
		if(not oldFadingState)
		{
			m_windStrengthSpr.setTextureRect({0, 0, 0, 0});//Don't display the arrow, this is the size limit
			m_windFadingTimer.restart();
		}
		//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
		sf::Time time{cap(m_windFadingTimer.getElapsedTime()-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
		sf::Color color{fadingColor(time, sf::seconds(3), false)};
		m_windStrengthBarSpr.setColor(color);
		m_windStrengthSpr.setColor(color);
	}
	//If the fading is not active
	else if(not m_windIsFading)
	{
		//If the fading end right now
		if(oldFadingState)
		{
			//Reset sprite colors
			m_windStrengthSpr.setColor(sf::Color::White);
			m_windStrengthBarSpr.setColor(sf::Color::White);
		}
		m_windStrengthSpr.setTextureRect({scaleRes<int>(120.f*(1.f - std::abs(windStrength)), scale), 0, scaleRes<int>(120.f*std::abs(windStrength), scale), scaleRes(20, scale)});
		if(windStrength > 0)
			m_windStrengthSpr.setScale(1, 1);
		else
			m_windStrengthSpr.setScale(-1, 1);//Flip the arrow
	}
	if(m_healthIsFading)
	{
		//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
		sf::Time time{cap(m_healthFadingTimer.getElapsedTime()-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
		sf::Color color{fadingColor(time, sf::seconds(3), false)};
		m_healthSpr.setColor(color);
		m_healthBorderSpr.setColor(color);
	}
	if(m_staminaIsFading)
	{
		//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
		sf::Time time{cap(m_staminaFadingTimer.getElapsedTime()-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
		sf::Color color{fadingColor(time, sf::seconds(3), false)};
		m_staminaSpr.setColor(color);
		m_staminaBorderSpr.setColor(color);
	}
	return true;
}

bool HUDState::handleEvent(const sf::Event&)
{
	return true;
}

void HUDState::receive(const PlayerHealthChange& playerHealthChange)
{
	bool oldFadingState{m_healthIsFading};
	//The fading state is true if the health is maximum
	m_healthIsFading = playerHealthChange.normalizedHealth >= 1.f;
	//If the fading start right now
	if(not oldFadingState and m_healthIsFading)
		m_healthFadingTimer.restart();
	//If the fading end now
	else if(oldFadingState and not m_healthIsFading)
	{
		//Reset sprite colors
		m_healthSpr.setColor(sf::Color::White);
		m_healthBorderSpr.setColor(sf::Color::White);
	}
	float scale{getContext().parameters.scale};
	m_healthSpr.setTextureRect({scaleRes<int>(240.f - 240.f*playerHealthChange.normalizedHealth, scale), 0, scaleRes(240, scale), scaleRes(20, scale)});
}

void HUDState::receive(const PlayerStaminaChange& playerStaminaChange)
{
	bool oldFadingState{m_staminaIsFading};
	//The fading state is true if the stamina is maximum
	m_staminaIsFading = playerStaminaChange.normalizedStamina >= 1.f;
	//If the fading start right now
	if(not oldFadingState and m_staminaIsFading)
		m_staminaFadingTimer.restart();
	//If the fading end now
	else if(oldFadingState and not m_staminaIsFading)
	{
		//Reset sprite colors
		m_staminaSpr.setColor(sf::Color::White);
		m_staminaBorderSpr.setColor(sf::Color::White);
	}
	m_staminaFadingTimer.restart();
	float scale{getContext().parameters.scale};
	m_staminaSpr.setTextureRect({scaleRes<int>(240 - 240.f*playerStaminaChange.normalizedStamina, scale), 0, scaleRes(240, scale), scaleRes(20, scale)});
}
