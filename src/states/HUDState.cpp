#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>

#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/ResourceManager.h>

#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState(StateStack& stack, Context context):
	State(stack, context),
	m_healthBar{nullptr},
	m_staminaBar{nullptr},
	m_windBar{nullptr},
	m_healthSpr{},
	m_healthBorderSpr{},
	m_staminaSpr{},
	m_staminaBorderSpr{},
	m_windStrengthSpr{},
	m_windStrengthBarSpr{},
	m_healthIsFading{false},
	m_staminaIsFading{false},
	m_windIsFading{false}
{
	getContext().eventManager.subscribe<PlayerHealthChange>(*this);
	getContext().eventManager.subscribe<PlayerStaminaChange>(*this);
	
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	float scale{getContext().parameters.scale};
	TextureManager& texManager(getContext().textureManager);
	
	//Load textures
	if(not texManager.isLoaded("health ath"))
		texManager.load("health ath", paths[getContext().parameters.scaleIndex] + "healthAth.png");
	if(not texManager.isLoaded("health border ath"))
		texManager.load("health border ath", paths[getContext().parameters.scaleIndex] + "healthBorderAth.png");
	if(not texManager.isLoaded("stamina ath"))
		texManager.load("stamina ath", paths[getContext().parameters.scaleIndex] + "staminaAth.png");
	if(not texManager.isLoaded("stamina border ath"))
		texManager.load("stamina border ath", paths[getContext().parameters.scaleIndex] + "staminaBorderAth.png");
	if(not texManager.isLoaded("wind arrow ath"))
		texManager.load("wind arrow ath", paths[getContext().parameters.scaleIndex] + "windArrowAth.png");
	if(not texManager.isLoaded("wind bar ath"))
		texManager.load("wind bar ath", paths[getContext().parameters.scaleIndex] + "windBarAth.png");
	
	//Set textures to sprites
	m_healthSpr.setTexture(texManager.get("health ath"));
	m_healthSpr.setTextureRect({0, 0, 240.f*scale, 20.f*scale});
	m_healthBorderSpr.setTexture(texManager.get("health border ath"));
	m_staminaSpr.setTexture(texManager.get("stamina ath"));
	m_staminaSpr.setTextureRect({0, 0, 240.f*scale, 20.f*scale});
	m_staminaBorderSpr.setTexture(texManager.get("stamina border ath"));
	m_windStrengthSpr.setTexture(texManager.get("wind arrow ath"));
	m_windStrengthBarSpr.setTexture(texManager.get("wind bar ath"));
	
	m_healthBar = tgui::Canvas::create({480*scale, 20*scale});
	m_healthBar->setPosition(bindWidth(gui, 0.01), bindHeight(gui, 0.99) - bindHeight(m_healthBar));
	gui.add(m_healthBar);
	m_staminaBar = tgui::Canvas::create({480*scale, 20*scale});
	m_staminaBar->setPosition(bindWidth(gui, 0.99) - bindWidth(m_staminaBar), bindHeight(gui, 0.99) - bindHeight(m_staminaBar));
	gui.add(m_staminaBar);
	m_windBar = tgui::Canvas::create({480*scale, 20*scale});
	m_windBar->setPosition(bindWidth(gui, 0.99) - bindWidth(m_windBar), bindHeight(gui, 0.99) - bindHeight(m_windBar) - bindHeight(m_staminaBar));
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
	return true;
}

bool HUDState::handleEvent(const sf::Event&)
{
	return true;
}

void HUDState::receive(const PlayerHealthChange& playerHealthChange)
{
	m_healthIsFading = false;
	float scale{getContext().parameters.scale};
	m_healthSpr.setTextureRect({(240.f - (240.f*playerHealthChange.normalizedHealth))*scale, 0, 240.f*scale, 20.f*scale});
}

void HUDState::receive(const PlayerStaminaChange& playerStaminaChange)
{
	m_staminaIsFading = false;
	float scale{getContext().parameters.scale};
	m_staminaSpr.setTextureRect({(240.f - (240.f*playerStaminaChange.normalizedStamina))*scale, 0, 240.f*scale, 20.f*scale});
}
