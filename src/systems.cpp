#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/constants.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/Animations.h>

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

void JumpSystem::update(entityx::EntityManager& entityManager, entityx::EventManager &eventManager, double)
{
	AnimationsComponent::Handle animationsComponent;
	Jump::Handle jumpComponent;
	Body::Handle bodyComponent;
	Direction::Handle direction;
	for(auto entity : entityManager.entities_with_components(animationsComponent, jumpComponent, bodyComponent, direction))
	{
		Animations* animations = animationsComponent->animations;
		b2Body* body = bodyComponent->body;
		if(jumpComponent->isJumping)
		{
			float32 vy = body->GetLinearVelocity().y;
				std::cout << vy<< std::endl;
			if(vy == 0.f)//On touch the ground
			{
				std::cout << "GND"<< std::endl;
				jumpComponent->isJumping = false;
				if(direction->toLeft)
					animations->play("stayLeft");
				else
					animations->play("stayRight");
			}
			else if(vy < 0.f)//Falling
			{
				if(direction->toLeft)
					animations->play("fallLeft");
				else
					animations->play("fallRight");
			}
			else if(vy > 0.f and direction->toLeft and  animations->getCurrentAnimation() == "jumpRight")//If diriged to left, set the right animation
			{
				float progress = animations->getProgress();
				animations->play("jumpLeft");
				animations->setProgress(progress);
			}
			else if(vy > 0.f and not direction->toLeft and  animations->getCurrentAnimation() == "jumpLeft")//If diriged to right, set the right animation
			{
				float progress = animations->getProgress();
				animations->play("jumpRight");
				animations->setProgress(progress);
			}
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