#include <cmath>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Window/Event.hpp>
#include <Box2D/Common/b2Math.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>
#include <TheLostGirl/systems/DragAndDropSystem.h>


DragAndDropSystem::DragAndDropSystem():
	m_origin{0, 0},
	m_line{sf::Vertex({0, 0}, sf::Color::Black), sf::Vertex({0, 0}, sf::Color::Black)},
	m_isActive{false}
{}

void DragAndDropSystem::update(entityx::EntityManager&, entityx::EventManager&, double)
{
	//If the drag and drop is active and effective
	if(m_isActive and m_origin != sf::Mouse::getPosition(*Context::window))
	{
		if(Context::parameters->bloomEnabled)
		{
			m_line[0].position = Context::postEffectsTexture->mapPixelToCoords(m_origin);
			m_line[1].position = Context::postEffectsTexture->mapPixelToCoords(sf::Mouse::getPosition(*Context::window));
			Context::postEffectsTexture->draw(m_line, 2, sf::Lines);
		}
		else
		{
			m_line[0].position = Context::window->mapPixelToCoords(m_origin);
			m_line[1].position = Context::window->mapPixelToCoords(sf::Mouse::getPosition(*Context::window));
			Context::window->draw(m_line, 2, sf::Lines);
		}
		//Compute the drag and drop data
		sf::Vector2f delta{m_line[1].position - m_line[0].position};
		float power{std::hypot(delta.x, delta.y)};//Distance between the two points
		float angle{std::atan2(delta.x, delta.y)};//Angle of the line with the horizontal axis
		angle += b2_pi/2.f;//Turn the angle of 90 degrees to fit the gameplay requirements
		//Send a command to player's entities to bend them bows according to the drag and drop data
		Command bendCommand;
		bendCommand.targetIsSpecific = false;
		bendCommand.category = Category::Player;
		bendCommand.action = BowBender(angle, power);
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(bendCommand);
	}
}

void DragAndDropSystem::setDragAndDropActivation(bool isActive)
{
	if(not m_isActive and isActive)//Activation
		m_origin = sf::Mouse::getPosition(*Context::window);
	if(not isActive and m_isActive)//Desactivation
	{
		float delta_x{m_line[1].position.x- m_line[0].position.x};
		float delta_y{m_line[1].position.y- m_line[0].position.y};
		float angle{std::atan2(delta_x, delta_y) + b2_pi/2.f};//Angle of the line with the horizontal axis
		//Shoot the arrow
		Command shootCommand;
		shootCommand.targetIsSpecific = false;
		shootCommand.category = Category::Player;
		shootCommand.action = ArrowShooter();
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(shootCommand);
		//Reset the bending power to 0
		Command bendCommand;
		bendCommand.targetIsSpecific = false;
		bendCommand.category = Category::Player;
		bendCommand.action = BowBender(float(angle), 0.f);//Reset the power of the bending
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.push(bendCommand);
	}
	m_isActive = isActive;
}
