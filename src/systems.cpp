#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Animations.h>

#include <TheLostGirl/systems.h>

using namespace sf;

void Actions::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, double dt)
{
	while(not m_commandQueue.empty())
	{
		Command& command = m_commandQueue.front();
		CategoryComponent::Handle categoryComponent;
		if(command.targetIsSpecific)
			command.action(*(command.entity), dt);
		else
		{
			for(auto entity : entityManager.entities_with_components(categoryComponent))
			{
				//On vérifie si l'entité correspond à la commande, si oui on fait l'action
				if(categoryComponent->category & command.category)
					command.action(entity, dt);
			}
		}
		m_commandQueue.pop();
	}
}

void JumpSystem::update(entityx::EntityManager& entityManager, entityx::EventManager &eventManager, double)
{
	AnimationsComponent::Handle animationsComponent;
	Jump::Handle jumpComponent;
	FallComponent::Handle fallComponent;
	Body::Handle bodyComponent;
	DirectionComponent::Handle directionComponent;
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															jumpComponent,
															bodyComponent,
															directionComponent,
															fallComponent))
	{//For every entity that can jump, set the right animation if it goes up
		Animations* animations = animationsComponent->animations;
		b2Body* body = bodyComponent->body;
		if(fallComponent->inAir and body->GetLinearVelocity().y < 0.f)
		{
			if(directionComponent->direction == Direction::Left)
				animations->play("fallLeft");
			else if(directionComponent->direction == Direction::Right)
				animations->play("fallRight");
		}
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
	AnimationsComponent::Handle anims;
	for(auto entity : entityManager.entities_with_components(anims))
		anims->animations->update(entity, sf::seconds(dt));
}