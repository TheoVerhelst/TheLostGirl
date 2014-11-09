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
#include <TheLostGirl/functions.h>

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

void ScrollingSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	if(m_levelRect != sf::IntRect(0, 0, 0, 0))
	{
		Body::Handle bodyComponent;
		CategoryComponent::Handle categoryComponent;
		Walk::Handle walkComponent;
		SpriteComponent::Handle spriteComponent;
		entityx::Entity playerEntity;
		bool found{false};
		for(auto entity : entityManager.entities_with_components(bodyComponent,
																categoryComponent,
																walkComponent))
		{
			found = categoryComponent->category & Category::Player;
			if(found)
			{
				playerEntity = entity;
				break;
			}
		}
		if(found)
		{
			Vector2f position = b2tosf(m_parameters.pixelScale * playerEntity.component<Body>()->body->GetPosition());
			View view{m_window.getView()};
			//Compute the maximum and minimum coordinates that the view can have
			float xmin{m_levelRect.left*m_parameters.scale + (view.getSize().x / 2)};
			float xmax{m_levelRect.left*m_parameters.scale + m_levelRect.width*m_parameters.scale - (view.getSize().x / 2)};
			float ymin{m_levelRect.top*m_parameters.scale + (view.getSize().y / 2)};
			float ymax{m_levelRect.top*m_parameters.scale + m_levelRect.height*m_parameters.scale - (view.getSize().y / 2)};
			//Cap the position between min and max
			position.x = cap(position.x, xmin, xmax);
			position.y = cap(position.y, ymin, ymax);
			
			//Assign position on every entity Sprite
			for(auto entity : entityManager.entities_with_components(spriteComponent))
			{
				//The number of the layer where is the entity
				float z = spriteComponent->worldPosition.z;
				//The x-ordinate of the left border of the screen.
				float xScreen = position.x - xmin;
				//So the abscissa of the entity on the screen, relatively to the reference plan and the position of the player
				float xScaled = spriteComponent->worldPosition.x + xScreen - (xScreen * pow(1.5, m_referencePlan - z));
				spriteComponent->sprite->setPosition(xScaled, spriteComponent->worldPosition.y);
			}
			
			//Assign the position to the view
			view.setCenter(position);
			m_window.setView(view);
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
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
		m_line[0].position = m_window.mapPixelToCoords(m_origin);
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
		m_origin = sf::Mouse::getPosition(m_window);
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
	//A map containing all entities grouped and sorted by z-order
	std::map<float, std::deque<entityx::Entity> > orderedEntities;
	//Add the entities in the map
	for(entityx::Entity entity : entityManager.entities_with_components(spriteComponent))
	{
		//If this is a scene entity, add it beyond the others entities in this plan
		if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Scene)
			orderedEntities[spriteComponent->worldPosition.z].push_front(entity);
		else
			orderedEntities[spriteComponent->worldPosition.z].push_back(entity);
	}
	//For each plan, in the reverse order
	for(std::multimap<float, std::deque<entityx::Entity> >::const_reverse_iterator it{orderedEntities.crbegin()}; it != orderedEntities.crend(); it++)
	{
		//Draw the entities of this plan
		for(auto entity : it->second)
			m_window.draw(*entity.component<SpriteComponent>()->sprite);
	}
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
		spriteComponent->worldPosition.x = pos.x * m_parameters.pixelScale;
		spriteComponent->worldPosition.y = pos.y * m_parameters.pixelScale;
		spriteComponent->sprite->setRotation(angle*180/b2_pi);
	}
}

void AnimationSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	AnimationsComponent::Handle anims;
	for(auto entity : entityManager.entities_with_components(anims))
		anims->animations->update(entity, sf::seconds(dt));
}