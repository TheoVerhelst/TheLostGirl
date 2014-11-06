#include <algorithm>
#include <cmath>
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Animations.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/systems.h>

using namespace sf;

void Actions::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
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

void FallSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	AnimationsComponent::Handle animationsComponent;
	FallComponent::Handle fallComponent;
	Body::Handle bodyComponent;
	DirectionComponent::Handle directionComponent;
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															bodyComponent,
															directionComponent,
															fallComponent))
	{//For every entity that can fall, set the right animation
		Animations* animations = animationsComponent->animations;
		b2Body* body = bodyComponent->body;
		if(fallComponent->inAir and body->GetLinearVelocity().y > 2.f)
		{
			if(directionComponent->direction == Direction::Left)
				animations->play("fall left");
			else if(directionComponent->direction == Direction::Right)
				animations->play("fall right");
		}
	}
}

void BendSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	AnimationsComponent::Handle animationsComponent;
	BendComponent::Handle bendComponent;
	DirectionComponent::Handle directionComponent;
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															bendComponent,
															directionComponent))
	{//For every entity that can bend a bow, set the right animation
		Animations* animations = animationsComponent->animations;
		float newProgress = bendComponent->power / bendComponent->maxPower;
		if(directionComponent->direction == Direction::Left)
			animations->setProgress("bend left", newProgress);
		else if(directionComponent->direction == Direction::Right)
			animations->setProgress("bend right", newProgress);
	}
}

void DragAndDropSystem::update(entityx::EntityManager&, entityx::EventManager&, double)
{
	if(m_isActive)
	{
		m_line[1].position = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
		m_window.draw(m_line, 2, sf::Lines);
		//Compute the drag and drop data
		float delta_x = m_line[1].position.x- m_line[0].position.x;
		float delta_y = m_line[1].position.y- m_line[0].position.y;
		float power = hypot(delta_x, delta_y);//Distance between the two points
		float angle = atan2(delta_x, delta_y);//Angle of the line with the horizontal axis
		angle += b2_pi/2.f;//Turn the angle of 90 degrees to fit with the gameplay requirements
		if(angle > b2_pi + b2_pi/4.f)//Keep the angle in the range [-3*pi/4, 5*pi/4]
			angle = angle - 2*b2_pi;
		//Send a command to player's entities to bend them bows according to the drag and drop data
		Command bendCommand;
		bendCommand.targetIsSpecific = false;
		bendCommand.category = Category::Player;
		bendCommand.action = BowBender(angle, power);
		m_commandQueue.push(bendCommand);
		m_window.draw(m_line, 2, sf::Lines);
	}
}

void DragAndDropSystem::setDragAndDropActivation(bool isActive)
{
	if(not m_isActive and isActive)//Activation
		m_line[0].position = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
	if(not isActive and m_isActive)//Desactivation
	{
		float delta_x = m_line[1].position.x- m_line[0].position.x;
		float delta_y = m_line[1].position.y- m_line[0].position.y;
		float angle = atan2(delta_x, delta_y) + b2_pi/2.f;//Angle of the line with the horizontal axis
		Command bendCommand;
		bendCommand.targetIsSpecific = false;
		bendCommand.category = Category::Player;
		bendCommand.action = BowBender(angle, 0.f);//Reset the power of the bending
		m_commandQueue.push(bendCommand);
	}
	m_isActive = isActive;
}

void Render::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	SpriteComponent::Handle spriteComponent;
	for(auto entity : entityManager.entities_with_components(spriteComponent))
		m_window.draw(*spriteComponent->sprite);
}

void Physics::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	m_world.Step(dt, velocityIterations, positionIterations);
	Body::Handle bodyComponent;
	SpriteComponent::Handle spriteComponent;
	Walk::Handle walkComponent;
	BendComponent::Handle bendComponent;
	//Update the walkers
	for(auto entity : entityManager.entities_with_components(bodyComponent, walkComponent))
	{
		b2Body* body = bodyComponent->body;
		float velocity = 0;
		if(walkComponent->effectiveMovement ==  Direction::Left)
			velocity = -walkComponent->walkSpeed;
		else if(walkComponent->effectiveMovement ==  Direction::Right)
			velocity = walkComponent->walkSpeed;
		body->SetLinearVelocity({velocity, body->GetLinearVelocity().y});
	}
	//Update the archers
	for(auto entity : entityManager.entities_with_components(bodyComponent, bendComponent))
	{
		b2Body* body = bodyComponent->body;
		b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(body->GetJointList()->joint);
		float angleTarget{bendComponent->angle};
		float32 angleError = joint->GetJointAngle() - angleTarget;
		float32 gain = 10.f;
		joint->SetMotorSpeed(-gain * angleError);
	}
	//Update the sprite transformation according to the one of the b2Body.
	for(auto entity : entityManager.entities_with_components(bodyComponent, spriteComponent))
	{
		b2Vec2 pos = bodyComponent->body->GetPosition();
		float32 angle = bodyComponent->body->GetAngle();
		spriteComponent->sprite->setPosition(pos.x * m_parameters.pixelScale, pos.y * m_parameters.pixelScale);
		spriteComponent->sprite->setRotation(angle*180/b2_pi);
	}
}

void AnimationSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	AnimationsComponent::Handle anims;
	for(auto entity : entityManager.entities_with_components(anims))
		anims->animations->update(entity, sf::seconds(dt));
}