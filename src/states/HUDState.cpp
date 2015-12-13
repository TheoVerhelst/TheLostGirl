#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/System.h>
#include <entityx/Entity.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/systems/TimeSystem.h>
#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState():
	m_windIsFading{true}
{
	Context::eventManager->subscribe<EntityHealthChange>(*this);
	Context::eventManager->subscribe<EntityStaminaChange>(*this);

	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(*Context::gui);

	//Load textures
	Context::textureManager->load("wind arrow ath", Context::parameters->imagePath + "windArrowAth.png");
	Context::textureManager->load("wind bar ath", Context::parameters->imagePath + "windBarAth.png");

	//Create the wind bar
	m_windStrengthSpr.setTexture(Context::textureManager->get("wind arrow ath"));
	m_windStrengthSpr.setPosition(120.f, 0);
	m_windStrengthBarSpr.setTexture(Context::textureManager->get("wind bar ath"));
	m_windBar = tgui::Canvas::create({std::ceil(240.f), std::ceil(20.f)});
	m_windBar->setPosition(bindWidth(gui, 0.99f) - bindWidth(m_windBar), bindHeight(gui, 0.95f) - bindHeight(m_windBar));
	gui.add(m_windBar);
}

HUDState::~HUDState()
{
	for(auto& barPair : m_healthBars)
		Context::gui->remove(barPair.second.panel);
	for(auto& barPair : m_staminaBars)
		Context::gui->remove(barPair.second.panel);
	Context::gui->remove(m_windBar);
}

void HUDState::draw()
{
	std::list<entityx::Entity> entitiesToRemove;
	for(auto& barPair : m_healthBars)
	{
		const auto deathComponent(barPair.first.component<DeathComponent>());
		if(deathComponent and deathComponent->dead)
		{
			Context::gui->remove(barPair.second.panel);
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
			Context::gui->remove(barPair.second.panel);
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
	const float windStrength{cap(Context::systemManager->system<TimeSystem>()->getWindStrength()/5.f, -1.f, 1.f)};
	sf::Vector2f viewPos;
	if(Context::parameters->bloomEnabled)
		viewPos = Context::postEffectsTexture->getView().getCenter() - Context::postEffectsTexture->getDefaultView().getCenter();
	else
		viewPos = Context::window->getView().getCenter() - Context::window->getDefaultView().getCenter();

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
			barPair.second.panel->setTransparency(alpha);
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
			barPair.second.panel->setTransparency(alpha);
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
	tgui::Gui& gui(*Context::gui);
	entityx::Entity entity{entityHealthChange.entity};

	if(not m_healthBars.count(entity))
	{
		Bar bar;
		if(isPlayer(entity))
		{
			bar.bar = tgui::Picture::create(Context::parameters->imagePath + "healthAth.png");
			bar.borders = tgui::Picture::create(Context::parameters->imagePath + "healthBorderAth.png");
			bar.panel = tgui::Panel::create({240+1, 20+1});
			bar.panel->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.99f) - bindHeight(bar.panel));
		}
		else
		{
			bar.bar = tgui::Picture::create(Context::parameters->imagePath + "entityHealthBar.png");
			bar.borders = tgui::Picture::create(Context::parameters->imagePath + "entityHealthBarBorders.png");
			bar.panel = tgui::Panel::create({100+1, 10+1});
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
		bar.panel->setTransparency(255);
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
	tgui::Gui& gui(*Context::gui);
	entityx::Entity entity{entityStaminaChange.entity};

	if(isPlayer(entity))
	{
		if(not m_staminaBars.count(entity))
		{
			Bar bar;
			bar.bar = tgui::Picture::create(Context::parameters->imagePath + "staminaAth.png");
			bar.borders = tgui::Picture::create(Context::parameters->imagePath + "staminaBorderAth.png");
			bar.panel = tgui::Panel::create({240, 20+1});
			bar.panel->setPosition(bindWidth(gui, 0.99f) - bindWidth(bar.panel), bindHeight(gui, 0.99f) - bindHeight(bar.panel));
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
			bar.panel->setTransparency(255);
		bar.timer = sf::Time::Zero;//In all cases, the timer should reset when the health change
		bar.bar->setPosition(std::ceil(-240.f*(1-entityStaminaChange.normalizedStamina)), 0.f);
	}
}

HUDState::Bar::~Bar()
{
}
