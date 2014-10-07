#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <algorithm>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/Player.h>

class Actions : public entityx::System<Actions>
{
	public:
		Actions(CommandQueue& commandQueue):
			m_commandQueue(commandQueue)
		{}
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
 
	private:
		CommandQueue& m_commandQueue;
};

class Render : public entityx::System<Render>
{
	public:
		Render(sf::RenderWindow& window):
			m_window(window)
		{}
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;
};

class Physics : public entityx::System<Physics>
{
	public:
		Physics(b2World& world):
			m_world(world)
		{}
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;
};

#endif // SYSTEMS_H