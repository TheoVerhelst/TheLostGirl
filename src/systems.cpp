#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/Command.h>

#include <TheLostGirl/systems.h>

using namespace sf;

void Actions::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, double dt)
{
	while(not m_commandQueue.empty())
	{
		Command command = m_commandQueue.front();
		Controller::Handle controller;
		for(auto entity : entityManager.entities_with_components(controller))
		{
			//On vérifie si l'entité correspond à la commande, si oui on fait l'action
			if(controller->isPlayer)
			{
				command.action(entity, dt);
				break;
			}
		}
		m_commandQueue.pop();
	}
}

void Render::update(entityx::EntityManager& entityManager, entityx::EventManager &eventManager, double)
{
	SpriteComponent::Handle spriteComponent;
	for(auto entity : entityManager.entities_with_components(spriteComponent))
		m_window.draw(spriteComponent->sprite);
}

void Physics::update(entityx::EntityManager& entityManager, entityx::EventManager &eventManager, double dt)
{
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	m_world.Step(dt, velocityIterations, positionIterations);
	Body::Handle bodyComponent;
	SpriteComponent::Handle spriteComponent;
	for(auto entity : entityManager.entities_with_components(bodyComponent, spriteComponent))
	{
		b2Vec2 pos = bodyComponent->body->GetPosition();
		spriteComponent->sprite.setPosition(pos.x * pixelScale, windowSize.y - (pos.y * pixelScale));
	}
}