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
#include <TheLostGirl/LangManager.h>

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

void SkySystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double spentTime)
{
	SkyComponent::Handle skyComponent;
	SpriteComponent::Handle spriteComponent;
	spentTime = remainder(spentTime, 600.f);//Get elapsed time since midnight
	
	for(auto entity : entityManager.entities_with_components(skyComponent, spriteComponent))
	{
		spriteComponent->sprite->setRotation(spentTime*0.6f);
		if(skyComponent->day)
		{
			if(spentTime < 112.5 or spentTime >= 487.5)//Night
				spriteComponent->sprite->setColor(Color(255, 255, 255, 0));
			else if(spentTime >= 112.5 and spentTime < 187.5)//Dawn
				spriteComponent->sprite->setColor(Color(255, 255, 255, ((spentTime - 112.5) / 75) * 255));
			else if(spentTime >= 187.5 and spentTime < 412.5)//Day
				spriteComponent->sprite->setColor(Color(255, 255, 255, 255));
			else if(spentTime >= 412.5 and spentTime < 487.5)//Twilight
				spriteComponent->sprite->setColor(Color(255, 255, 255, (1 - ((spentTime - 412.5) / 75)) * 255));
		}
		else
		{
			if(spentTime < 112.5 or spentTime >= 487.5)//Night
				spriteComponent->sprite->setColor(Color(255, 255, 255, 255));
			else if(spentTime >= 112.5 and spentTime < 187.5)//Dawn
				spriteComponent->sprite->setColor(Color(255, 255, 255, (1 - ((spentTime - 112.5) / 75)) * 255));
			else if(spentTime >= 187.5 and spentTime < 412.5)//Day
				spriteComponent->sprite->setColor(Color(255, 255, 255, 0));
			else if(spentTime >= 412.5 and spentTime < 487.5)//Twilight
				spriteComponent->sprite->setColor(Color(255, 255, 255, ((spentTime - 412.5) / 75) * 255));
		}
	}
}

void ScrollingSystem::setLevelData(const sf::IntRect& levelRect, float referencePlan)
{
	m_levelRect = levelRect;
	m_referencePlan = referencePlan;
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
	AnimationsComponent::Handle animationsComponent;
	FallComponent::Handle fallComponent;
	Body::Handle bodyComponent;
	DirectionComponent::Handle directionComponent;
	BendComponent::Handle bendComponent;
	
	//For every entity that can fall, set the right animation
	for(auto entity : entityManager.entities_with_components(animationsComponent,
															bodyComponent,
															directionComponent,
															fallComponent))
	{
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
	
	//Update the Animations components
	for(auto entity : entityManager.entities_with_components(animationsComponent))
		animationsComponent->animations->update(entity, sf::seconds(dt));
}

TimeSystem::~TimeSystem()
{}

void TimeSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	m_totalTime += dt;
	float seasonFactor = sin((m_totalTime / 24000.f) * 360.f); //Vary between -1  and 1, maximum in the spring and the automn
	seasonFactor = (seasonFactor + 1.f) / 2.f; //Now between 0 and 1

	/*
	The wind works by period of a random number of seconds, m_windTransitionLength,
	and choose a new random number, m_nextWindStrength, at every end of period,
	So the old m_nextWindStrength become m_initialWindStrength,
	and the wind tends to be progressively equal to m_nextWindStrength.
	The wind is also influenced by windStrengthSeasonFactor.
	This system is applied a second time, in smaller proportions.
	*/
	if(m_totalTime >= m_periodBeginning + m_windTransitionLength)//End of a period
	{
		m_periodBeginning = m_totalTime;
		m_initialWindStrength = m_nextWindStrength;
		m_windTransitionLength = static_cast<float>(rand() % 586000 + 15000) / 1000.f; //A random number between 15 and 600
		m_nextWindStrength = static_cast<float>(rand() % 21000 - 10000) / 1000.f; //Another random number, between -10 and 10
	}

	float deltaStrength = m_nextWindStrength - m_initialWindStrength;
	float elapsedTimeSincePeriodBeginning = m_totalTime - m_periodBeginning;
	m_windStrength = m_initialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_windTransitionLength);

	if(m_totalTime >= m_microPeriodBeginning + m_microWindTransitionLength)//End of a period
	{
		m_microPeriodBeginning = m_totalTime;
		m_microInitialWindStrength = m_microNextWindStrength;
		m_microWindTransitionLength = static_cast<float>(rand() % 1600 + 500) / 5000.f; //A random number between 0.5 and 2
		m_microNextWindStrength = static_cast<float>(rand() % 1000 - 2000) / 1000.f; //Another random number, between -1 and 1
	}

	deltaStrength = m_microNextWindStrength - m_microInitialWindStrength;
	elapsedTimeSincePeriodBeginning = m_totalTime - m_microPeriodBeginning;
	m_windStrength += m_microInitialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_microWindTransitionLength);
	m_windStrength *= seasonFactor;//Apply the factor of the season
}

void TimeSystem::setTotalTime(sf::Time totalTime)
{
	m_totalTime = totalTime.asSeconds();
}

std::wstring TimeSystem::seasonName() const
{
	switch(months() / 4)
	{
		case 0:
			return LangManager::tr("Snow season");
			break;
		case 1:
			return LangManager::tr("Flowers season");
			break;
		case 2:
			return LangManager::tr("Fruit season");
			break;
		case 3:
			return LangManager::tr("Wind season");
			break;
		default:
			return LangManager::tr("Snow season");
	}
}

std::wstring TimeSystem::monthName() const
{
	switch(months())
	{
		case 0:
			return LangManager::tr("snow month");
			break;
		case 1:
			return LangManager::tr("winds month");
			break;
		case 2:
			return LangManager::tr("buds month");
			break;
		case 3:
			return LangManager::tr("flowers month");
			break;
		case 4:
			return LangManager::tr("harvests month");
			break;
		case 5:
			return LangManager::tr("fruits month");
			break;
		case 6:
			return LangManager::tr("mists month");
			break;
		case 7:
			return LangManager::tr("month of frost");
			break;
		default:
			return LangManager::tr("snow month");
	}
}

std::wstring TimeSystem::getFormatedDate() const
{
	std::wstring output;
	//Season name
	output += seasonName() + L"\n";
	//Year name
	output += std::to_wstring(years() + 1);
	if(years() + 1 == 1)
		output += LangManager::tr("st year, ");
	else if(years() + 1 == 2)
		output += LangManager::tr("nd year, ");
	else
		output += LangManager::tr("th year, ");
	//Day name
	output += std::to_wstring(days() + 1);
	if(days() + 1 == 1)
		output += LangManager::tr("st day of the ");
	else if(days() + 1 == 2)
		output += LangManager::tr("nd day of the ");
	else
		output += LangManager::tr("th day of the ");
	//Month name
	output += monthName();
	return output;
}

std::wstring TimeSystem::getFormatedTime() const
{
	return std::to_wstring(hours()) + L":" + std::to_wstring(minutes());
}

unsigned TimeSystem::years() const
{
	return unsigned(m_totalTime) / 48000;
}

unsigned TimeSystem::months() const
{
	return (unsigned(m_totalTime) % 48000) / 6000;
}

unsigned TimeSystem::days() const
{
	return (unsigned(m_totalTime) % 6000) / 600;
}

unsigned TimeSystem::hours() const
{
	return (unsigned(m_totalTime) % 600) / 30;
}

unsigned TimeSystem::minutes() const
{
	return (unsigned(m_totalTime * 2) % 60);
}

sf::Time TimeSystem::getRealTime() const
{
	return sf::seconds(m_totalTime);
}

float TimeSystem::getWindStrength() const
{
	return m_windStrength;
}