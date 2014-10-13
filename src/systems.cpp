#include <algorithm>
#include <iostream>

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
		Command& command = m_commandQueue.front();
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
		m_window.draw(*spriteComponent->sprite);
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
		spriteComponent->sprite->setPosition(pos.x * pixelScale, windowSize.y - (pos.y * pixelScale));
	}
}

void AnimationSystem::update(entityx::EntityManager& entityManager, entityx::EventManager &eventManager, double dt)
{
	Animations::Handle anims;
	for(auto entity : entityManager.entities_with_components(anims))
	{
		std::string id = anims->currentAnimation;
		//If we must play an animation and if the current animation is in the map
		if(anims->isPlaying and anims->animations.find(id) != anims->animations.end())
		{
			Animations::TimeAnimation& timeAnim = anims->animations.at(id);//Access this element is safe
			anims->progress += dt/timeAnim.duration.asSeconds();
			if(anims->loops and anims->progress > 1.f)
				anims->progress -= std::floor(anims->progress);//Keep the progress in the range [0, 1]
			
			if(anims->progress > 1.f)
				anims->isPlaying = false;
			else
				timeAnim.animation(entity, anims->progress);
		}
	}
}