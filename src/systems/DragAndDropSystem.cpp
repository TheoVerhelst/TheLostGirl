#include <cmath>
#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <Box2D/Common/b2Math.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/Category.h>

#include <TheLostGirl/systems/DragAndDropSystem.h>

void DragAndDropSystem::update(entityx::EntityManager&, entityx::EventManager&, double)
{
	//If the drag and drop is active and effective
	if(m_isActive and m_origin != sf::Mouse::getPosition(m_window))
	{
		m_line[0].position = m_window.mapPixelToCoords(m_origin);
		m_line[1].position = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
		m_window.draw(m_line, 2, sf::Lines);
		//Compute the drag and drop data
		float delta_x{m_line[1].position.x- m_line[0].position.x};
		float delta_y{m_line[1].position.y- m_line[0].position.y};
		double power{hypot(delta_x, delta_y)};//Distance between the two points
		double angle{atan2(delta_x, delta_y)};//Angle of the line with the horizontal axis
		angle += b2_pi/2.f;//Turn the angle of 90 degrees to fit with the gameplay requirements
//		if(angle > b2_pi + b2_pi/4.f)//Keep the angle in the range [-pi, pi]
//			angle = angle - 2*b2_pi;
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
		float delta_x{m_line[1].position.x- m_line[0].position.x};
		float delta_y{m_line[1].position.y- m_line[0].position.y};
		double angle{atan2(delta_x, delta_y) + b2_pi/2.f};//Angle of the line with the horizontal axis
		//Shoot the arrow
		Command shootCommand;
		shootCommand.targetIsSpecific = false;
		shootCommand.category = Category::Player;
		shootCommand.action = ArrowShooter();
		m_commandQueue.push(shootCommand);
		//Reset the bending power to 0
		Command bendCommand;
		bendCommand.targetIsSpecific = false;
		bendCommand.category = Category::Player;
		bendCommand.action = BowBender(angle, 0.f);//Reset the power of the bending
		m_commandQueue.push(bendCommand);
		//Do these two commands in this order will first shoot the arrow with the current angle/power
		//in the BendComponent, and then reset the BendComponent's power to 0
	}
	m_isActive = isActive;
}
