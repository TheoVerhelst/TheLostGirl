#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/LangManager.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/ResourceManager.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/systems/TimeSystem.hpp>
#include <TheLostGirl/states/HUDState.hpp>

HUDState::HUDState():
	m_windIsFading{true}
{
	getEventManager().subscribe<EntityHealthChange>(*this);
	getEventManager().subscribe<EntityStaminaChange>(*this);

	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getGui());

	//Load textures
	getTextureManager().load("wind arrow ath", getParameters().resourcesPath + "images/windArrowAth.png");
	getTextureManager().load("wind bar ath", getParameters().resourcesPath + "images/windBarAth.png");

	//Create the wind bar
	m_windStrengthSpr.setTexture(getTextureManager().get("wind arrow ath"));
	m_windStrengthSpr.setPosition(120.f, 0);
	m_windStrengthBarSpr.setTexture(getTextureManager().get("wind bar ath"));
	m_windBar = std::make_shared<tgui::Canvas>(tgui::Layout2d(sf::Vector2f(std::ceil(240.f), std::ceil(20.f))));
	m_windBar->setPosition(bindWidth(gui) * 0.99f - bindWidth(m_windBar), bindHeight(gui) * 0.95f - bindHeight(m_windBar));
	gui.add(m_windBar);
}

HUDState::~HUDState()
{
	for(auto& barPair : m_healthBars)
		getGui().remove(barPair.second.panel);
	for(auto& barPair : m_staminaBars)
		getGui().remove(barPair.second.panel);
	getGui().remove(m_windBar);
}

void HUDState::draw()
{
	std::list<entityx::Entity> entitiesToRemove;
	for(auto& barPair : m_healthBars)
	{
		const auto deathComponent(barPair.first.component<DeathComponent>());
		if(deathComponent and deathComponent->dead)
		{
			getGui().remove(barPair.second.panel);
			entitiesToRemove.push_back(barPair.first);
		}
	}

	for(auto& entity : entitiesToRemove)
		m_healthBars.erase(entity);
	entitiesToRemove.clear();
	for(auto& barPair : m_staminaBars)
	{
		const auto deathComponent(barPair.first.component<DeathComponent>());
		if(deathComponent and deathComponent->dead)
		{
			getGui().remove(barPair.second.panel);
			entitiesToRemove.push_back(barPair.first);
		}
	}
	for(auto& entity : entitiesToRemove)
		m_staminaBars.erase(entity);

	m_windBar->clear(sf::Color::Transparent);
	m_windBar->draw(m_windStrengthBarSpr);
	m_windBar->draw(m_windStrengthSpr);
	m_windBar->display();
}

bool HUDState::update(sf::Time dt)
{
	//Get the wind strength and compute the wind ath
	const float windStrength{cap(getSystemManager().system<TimeSystem>()->getWindStrength()/5.f, -1.f, 1.f)};
	sf::Vector2f viewPos;
	if(getParameters().bloomEnabled)
		viewPos = getPostEffectsTexture().getView().getCenter() - getPostEffectsTexture().getDefaultView().getCenter();
	else
		viewPos = getWindow().getView().getCenter() - getWindow().getDefaultView().getCenter();

	for(auto& barPair : m_healthBars)
		barPair.second.timer += dt;
	for(auto& barPair : m_staminaBars)
		barPair.second.timer += dt;
	m_windFadingTimer += dt;

	const bool oldFadingState{m_windIsFading};
	m_windIsFading = std::abs(windStrength) < 0.125f;
	if(m_windIsFading)
	{
		//If the fading start now
		if(not oldFadingState)
		{
			m_windStrengthSpr.setTextureRect({0, 0, 0, 0});//Don't display the arrow, this is the size limit
			m_windFadingTimer = sf::Time::Zero;
		}
		//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
		const sf::Time time{cap(m_windFadingTimer-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
		const sf::Color color{fadingColor(time, sf::seconds(3), false)};
		m_windStrengthBarSpr.setColor(color);
		m_windStrengthSpr.setColor(color);
	}
	//If the fading is not active
	else
	{
		//If the fading end right now
		if(oldFadingState)
		{
			//Reset sprite colors
			m_windStrengthSpr.setColor(sf::Color::White);
			m_windStrengthBarSpr.setColor(sf::Color::White);
		}
		m_windStrengthSpr.setTextureRect({int(std::ceil(120.f*(1.f - std::abs(windStrength)))), 0, int(std::ceil(120.f*std::abs(windStrength))), int(std::ceil(20.f))});
		if(windStrength > 0)
			m_windStrengthSpr.setScale(1, 1);
		else
			m_windStrengthSpr.setScale(-1, 1);//Flip the arrow
	}
	for(auto& barPair : m_healthBars)
	{
		if(barPair.second.isFull)
		{
			//This time will be equal to 0 while 2 seconds after the start of the fading and then will grow normally
			const float time{cap(barPair.second.timer.asSeconds()-2.f, 0.f, 3.f)};
			const unsigned char alpha{(unsigned char)(765.f - time*255.f)};
			barPair.second.panel->setOpacity(alpha);
		}
		const auto transformComponent(barPair.first.component<TransformComponent>());
		if(transformComponent and not isPlayer(barPair.first))
		{
			sf::Vector2f position(transformComponent->transform.x, transformComponent->transform.y);
			position -= viewPos;
			barPair.second.panel->setPosition(position);
		}
	}
	for(auto& barPair : m_staminaBars)
	{
		if(barPair.second.isFull)
		{
			//This time will be equal to 0 while 2 seconds after the start of the fading and then will grow normally
			const float time{cap(barPair.second.timer.asSeconds()-2.f, 0.f, 3.f)};
			const unsigned char alpha{(unsigned char)(765.f - time*255.f)};
			barPair.second.panel->setOpacity(alpha);
		}
	}
	return true;
}

bool HUDState::handleEvent(const sf::Event&)
{
	return true;
}

void HUDState::receive(const EntityHealthChange& entityHealthChange)
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getGui());
	entityx::Entity entity{entityHealthChange.entity};

	if(not m_healthBars.count(entity))
	{
		Bar bar;
		if(isPlayer(entity))
		{
			bar.bar = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/healthAth.png");
			bar.borders = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/healthBorderAth.png");
			bar.panel = std::make_shared<tgui::Panel>(tgui::Layout2d(sf::Vector2f(240+1, 20+1)));
			bar.panel->setPosition(bindWidth(gui) * 0.01f, bindHeight(gui) * 0.99f - bindHeight(bar.panel));
		}
		else
		{
			bar.bar = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/entityHealthBar.png");
			bar.borders = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/entityHealthBarBorders.png");
			bar.panel = std::make_shared<tgui::Panel>(tgui::Layout2d(sf::Vector2f(100+1, 10+1)));
		}
		bar.panel->setBackgroundColor(sf::Color::Transparent);
		bar.panel->add(bar.bar);
		bar.panel->add(bar.borders);
		gui.add(bar.panel);
		m_healthBars.emplace(entity, bar);
	}
	Bar& bar(m_healthBars.at(entity));
	const bool barWasFull{bar.isFull};
	bar.isFull = entityHealthChange.normalizedHealth >= 1.f;
	//If the fading ends now
	if(barWasFull and not bar.isFull)
		bar.panel->setOpacity(255);
	bar.timer = sf::Time::Zero;//In all cases, the timer should reset when the health change
	if(isPlayer(entity))
		bar.bar->setPosition(std::ceil(-240.f*(1-entityHealthChange.normalizedHealth)), 0.f);
	else
		bar.bar->setPosition(std::ceil(-100.f*(1-entityHealthChange.normalizedHealth)), 0.f);
}

void HUDState::receive(const EntityStaminaChange& entityStaminaChange)
{
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getGui());
	entityx::Entity entity{entityStaminaChange.entity};

	if(isPlayer(entity))
	{
		if(not m_staminaBars.count(entity))
		{
			Bar bar;
			bar.bar = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/staminaAth.png");
			bar.borders = std::make_shared<tgui::Picture>(getParameters().resourcesPath + "images/staminaBorderAth.png");
			bar.panel = std::make_shared<tgui::Panel>(tgui::Layout2d(sf::Vector2f(240, 20+1)));
			bar.panel->setPosition(bindWidth(gui) * 0.99f - bindWidth(bar.panel), bindHeight(gui) * 0.99f - bindHeight(bar.panel));
			bar.panel->setBackgroundColor(sf::Color::Transparent);
			bar.panel->add(bar.bar);
			bar.panel->add(bar.borders);
			m_staminaBars.emplace(entity, bar);
		}
		Bar& bar(m_staminaBars.at(entity));
		const bool barWasFull{bar.isFull};
		bar.isFull = entityStaminaChange.normalizedStamina >= 1.f;
		//If the fading ends now
		if(barWasFull and not bar.isFull)
			bar.panel->setOpacity(255);
		bar.timer = sf::Time::Zero;//In all cases, the timer should reset when the health change
		bar.bar->setPosition(std::ceil(-240.f*(1-entityStaminaChange.normalizedStamina)), 0.f);
	}
}

HUDState::Bar::~Bar()
{
}
