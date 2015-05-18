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
	m_windBar{nullptr},
	m_windStrengthSpr{},
	m_windStrengthBarSpr{},
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
	texManager.load("entity health bar borders", paths[getContext().parameters.scaleIndex] + "entityHealthBarBorders.png");
	texManager.load("entity health bar", paths[getContext().parameters.scaleIndex] + "entityHealthBar.png");
	texManager.load("health ath", paths[getContext().parameters.scaleIndex] + "healthAth.png");
	texManager.load("health border ath", paths[getContext().parameters.scaleIndex] + "healthBorderAth.png");
	texManager.load("stamina ath", paths[getContext().parameters.scaleIndex] + "staminaAth.png");
	texManager.load("stamina border ath", paths[getContext().parameters.scaleIndex] + "staminaBorderAth.png");
	texManager.load("wind arrow ath", paths[getContext().parameters.scaleIndex] + "windArrowAth.png");
	texManager.load("wind bar ath", paths[getContext().parameters.scaleIndex] + "windBarAth.png");

	//Set textures to sprites
	m_windStrengthSpr.setTexture(texManager.get("wind arrow ath"));
	m_windStrengthSpr.setPosition(120.f*scale, 0);
	m_windStrengthBarSpr.setTexture(texManager.get("wind bar ath"));
	m_windBar = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
	m_windBar->setPosition(bindWidth(gui, 0.99f) - bindWidth(m_windBar), bindHeight(gui, 0.95f) - bindHeight(m_windBar));
	gui.add(m_windBar);
}

HUDState::~HUDState()
{
	tgui::Gui& gui(getContext().gui);
	for(auto& barPair : m_healthBars)
		gui.remove(barPair.second.canvas);
	for(auto& barPair : m_staminaBars)
		gui.remove(barPair.second.canvas);
	gui.remove(m_windBar);
}

void HUDState::draw()
{
	for(auto& barPair : m_healthBars)
	{
		barPair.second.canvas->clear(sf::Color::Transparent);
		barPair.second.canvas->draw(barPair.second.sprite);
		barPair.second.canvas->draw(barPair.second.borderSprite);
		barPair.second.canvas->display();
	}
	for(auto& barPair : m_staminaBars)
	{
		barPair.second.canvas->clear(sf::Color::Transparent);
		barPair.second.canvas->draw(barPair.second.sprite);
		barPair.second.canvas->draw(barPair.second.borderSprite);
		barPair.second.canvas->display();
	}
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
	const sf::RenderWindow& window(getContext().window);

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
		m_windStrengthSpr.setTextureRect({scaleRes<float, int>(120.f*(1.f - std::abs(windStrength)), scale), 0, scaleRes<float, int>(120.f*std::abs(windStrength), scale), scaleRes(20, scale)});
		if(windStrength > 0)
			m_windStrengthSpr.setScale(1, 1);
		else
			m_windStrengthSpr.setScale(-1, 1);//Flip the arrow
	}
	for(auto& barPair : m_healthBars)
	{
		if(barPair.second.isFull)
		{
			//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
			const sf::Time time{cap(barPair.second.timer-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
			const sf::Color color{fadingColor(time, sf::seconds(3), false)};
			barPair.second.sprite.setColor(color);
			barPair.second.borderSprite.setColor(color);
		}
		if(barPair.first.has_component<TransformComponent>() and not isPlayer(barPair.first))
		{
			const auto& transforms(barPair.first.component<TransformComponent>()->transforms);
			if(transforms.find("main") != transforms.end())
			{
				sf::Vector2f position(transforms.at("main").x, transforms.at("main").y);
				position *= scale;
				position -= window.getView().getCenter() - window.getDefaultView().getCenter();
				barPair.second.canvas->setPosition(position);
			}
		}
	}
	for(auto& barPair : m_staminaBars)
	{
		if(barPair.second.isFull)
		{
			//This time will be equal to 0 2 seconds after the start of the fading and then will grow normally
			const sf::Time time{cap(barPair.second.timer-sf::seconds(2), sf::seconds(0), sf::seconds(3))};
			const sf::Color color{fadingColor(time, sf::seconds(3), false)};
			barPair.second.sprite.setColor(color);
			barPair.second.borderSprite.setColor(color);
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

	if(m_healthBars.find(entity) == m_healthBars.end())
	{
		Bar bar;
		if(isPlayer(entity))
		{
			bar.sprite.setTexture(texManager.get("health ath"));
			bar.sprite.setTextureRect({0, 0, scaleRes(240, scale), scaleRes(20, scale)});
			bar.borderSprite.setTexture(texManager.get("health border ath"));
			bar.canvas = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
			bar.canvas->setPosition(bindWidth(gui, 0.01f), bindHeight(gui, 0.99f) - bindHeight(bar.canvas));
		}
		else
		{
			bar.sprite.setTexture(texManager.get("entity health bar"));
			bar.sprite.setTextureRect({0, 0, scaleRes(100, scale), scaleRes(10, scale)});
			bar.borderSprite.setTexture(texManager.get("entity health bar borders"));
			bar.canvas = tgui::Canvas::create({scaleRes<int, float>(100, scale), scaleRes<int, float>(10.f, scale)});
			bar.canvas->setPosition(bindWidth(gui, 0.5f), bindHeight(gui, 0.99f) - bindHeight(bar.canvas));
		}
		gui.add(bar.canvas);
		m_healthBars.emplace(entity, bar);
	}
	Bar& bar(m_healthBars[entity]);
	const bool oldfFullState{bar.isFull};
	bar.isFull = entityHealthChange.normalizedHealth >= 1.f;
	//If the fading ends now
	if(oldfFullState and not bar.isFull)
	{
		//Reset sprite colors
		bar.sprite.setColor(sf::Color::White);
		bar.borderSprite.setColor(sf::Color::White);
	}
	bar.timer = sf::Time::Zero;//In all cases, the timer should reset when the health change
	if(isPlayer(entity))
		bar.sprite.setTextureRect({scaleRes<float, int>(240.f - 240.f*entityHealthChange.normalizedHealth, scale), 0, scaleRes(240, scale), scaleRes(20, scale)});
	else
		bar.sprite.setTextureRect({scaleRes<float, int>(100.f - 100.f*entityHealthChange.normalizedHealth, scale), 0, scaleRes(100, scale), scaleRes(10, scale)});
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
		if(m_staminaBars.find(entity) == m_staminaBars.end())
		{
			Bar bar;
			bar.sprite.setTexture(texManager.get("stamina ath"));
			bar.sprite.setTextureRect({0, 0, scaleRes(240, scale), scaleRes(20, scale)});
			bar.borderSprite.setTexture(texManager.get("stamina border ath"));
			bar.canvas = tgui::Canvas::create({scaleRes(240.f, scale), scaleRes(20.f, scale)});
			bar.canvas->setPosition(bindWidth(gui, 0.99f) - bindWidth(bar.canvas), bindHeight(gui, 0.99f) - bindHeight(bar.canvas));
			gui.add(bar.canvas);
			m_staminaBars.emplace(entity, bar);
		}
		Bar& bar(m_staminaBars[entity]);
		const bool oldfFullState{bar.isFull};
		bar.isFull = entityStaminaChange.normalizedStamina >= 1.f;
		//If the fading ends now
		if(oldfFullState and not bar.isFull)
		{
			//Reset sprite colors
			bar.sprite.setColor(sf::Color::White);
			bar.borderSprite.setColor(sf::Color::White);
		}
		bar.timer = sf::Time::Zero;//In all cases, the timer should reset when the health change
		bar.sprite.setTextureRect({scaleRes<float, int>(240 - 240.f*entityStaminaChange.normalizedStamina, scale), 0, scaleRes(240, scale), scaleRes(20, scale)});
	}
}
