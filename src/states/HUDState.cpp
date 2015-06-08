#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Gui.hpp>
#include <entityx/System.h>
#include <entityx/Entity.h>
#include <TheLostGirl/State.h>
#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/systems/TimeSystem.h>
#include <TheLostGirl/states/HUDState.h>

HUDState::HUDState(StateStack& stack):
	State(stack),
	m_windIsFading{true}
{
	getContext().eventManager.subscribe<EntityHealthChange>(*this);
	getContext().eventManager.subscribe<EntityStaminaChange>(*this);

	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	const float scale{getContext().parameters.scale};
	TextureManager& texManager(getContext().textureManager);

	//Load textures
	texManager.load("wind arrow ath", paths[getContext().parameters.scaleIndex] + "windArrowAth.png");
	texManager.load("wind bar ath", paths[getContext().parameters.scaleIndex] + "windBarAth.png");

	//Create the wind bar
	m_windStrengthSpr.setTexture(texManager.get("wind arrow ath"));
	m_windStrengthSpr.setPosition(120.f*scale, 0);
	m_windStrengthBarSpr.setTexture(texManager.get("wind bar ath"));
	m_windBar = tgui::Canvas::create({std::ceil(240.f*scale), std::ceil(20.f*scale)});
	m_windBar->setPosition(bindWidth(gui, 0.99f) - bindWidth(m_windBar), bindHeight(gui, 0.95f) - bindHeight(m_windBar));
	gui.add(m_windBar);
}

HUDState::~HUDState()
{
	tgui::Gui& gui(getContext().gui);
	for(auto& barPair : m_healthBars)
		gui.remove(barPair.second.panel);
	for(auto& barPair : m_staminaBars)
		gui.remove(barPair.second.panel);
	gui.remove(m_windBar);
}

void HUDState::draw()
{
	std::list<entityx::Entity> entitiesToRemove;
	for(auto& barPair : m_healthBars)
	{
		if(barPair.first.has_component<DeathComponent>() and barPair.first.component<DeathComponent>()->dead)
		{
			getContext().gui.remove(barPair.second.panel);
			entitiesToRemove.push_back(barPair.first);
		}
	}

	for(auto& entity : entitiesToRemove)
		m_healthBars.erase(entity);
	entitiesToRemove.clear();
	for(auto& barPair : m_staminaBars)
	{
		if(barPair.first.has_component<DeathComponent>() and barPair.first.component<DeathComponent>()->dead)
		{
			getContext().gui.remove(barPair.second.panel);
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
	const float scale{getContext().parameters.scale};
	const float windStrength{cap(getContext().systemManager.system<TimeSystem>()->getWindStrength()/5.f, -1.f, 1.f)};
	sf::Vector2f viewPos;
	if(getContext().parameters.bloomEnabled)
		viewPos = getContext().postEffectsTexture.getView().getCenter() - getContext().postEffectsTexture.getDefaultView().getCenter();
	else
		viewPos = getContext().window.getView().getCenter() - getContext().window.getDefaultView().getCenter();

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
		m_windStrengthSpr.setTextureRect({int(std::ceil(120.f*(1.f - std::abs(windStrength))*scale)), 0, int(std::ceil(120.f*std::abs(windStrength)*scale)), int(std::ceil(20.f*scale))});
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
		if(barPair.first.has_component<TransformComponent>() and not isPlayer(barPair.first))
		{
			auto transformIt(barPair.first.component<TransformComponent>()->transforms.find("main"));
			if(transformIt != barPair.first.component<TransformComponent>()->transforms.end())
			{
				sf::Vector2f position(transformIt->second.x, transformIt->second.y);
				position *= scale;
				position -= viewPos;
				barPair.second.panel->setPosition(position);
			}
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
	TextureManager& texManager(getContext().textureManager);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	const float scale{getContext().parameters.scale};
	entityx::Entity entity{entityHealthChange.entity};

	if(not m_healthBars.count(entity))
	{
		Bar bar;
		if(isPlayer(entity))
		{
			bar.bar = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "healthAth.png");
			bar.borders = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "healthBorderAth.png");
			bar.panel = tgui::Panel::create({std::ceil(240.f*scale)+1, std::ceil(20.f*scale)+1});
			bar.panel->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.99f) - bindHeight(bar.panel));
		}
		else
		{
			bar.bar = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "entityHealthBar.png");
			bar.borders = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "entityHealthBarBorders.png");
			bar.panel = tgui::Panel::create({std::ceil(100.f*scale)+1, std::ceil(10.f*scale)+1});
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
		bar.bar->setPosition(std::ceil(-240.f*(1-entityHealthChange.normalizedHealth)*scale), 0.f);
	else
		bar.bar->setPosition(std::ceil(-100.f*(1-entityHealthChange.normalizedHealth)*scale), 0.f);
}

void HUDState::receive(const EntityStaminaChange& entityStaminaChange)
{
	TextureManager& texManager(getContext().textureManager);
	using tgui::bindWidth;
	using tgui::bindHeight;
	tgui::Gui& gui(getContext().gui);
	const float scale{getContext().parameters.scale};
	entityx::Entity entity{entityStaminaChange.entity};

	if(isPlayer(entity))
	{
		if(not m_staminaBars.count(entity))
		{
			Bar bar;
			bar.bar = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "staminaAth.png");
			bar.borders = tgui::Picture::create(paths[getContext().parameters.scaleIndex] + "staminaBorderAth.png");
			bar.panel = tgui::Panel::create({std::ceil(240.f*scale), std::ceil(20.f*scale)+1});
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
		bar.bar->setPosition(std::ceil(-240.f*(1-entityStaminaChange.normalizedStamina)*scale), 0.f);
	}
}
