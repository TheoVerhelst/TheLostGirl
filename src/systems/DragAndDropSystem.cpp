#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/Command.hpp>
#include <TheLostGirl/actions.hpp>
#include <TheLostGirl/Category.hpp>
#include <TheLostGirl/Parameters.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>
#include <TheLostGirl/systems/DragAndDropSystem.hpp>


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
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(FlagSet<Category>({Category::Player}), BowBender(angle, power));
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
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(FlagSet<Category>({Category::Player}), ArrowShooter());
		//Reset the power of the bending
		Context::systemManager->system<PendingChangesSystem>()->commandQueue.emplace(FlagSet<Category>({Category::Player}), BowBender(float(angle), 0.f));
	}
	m_isActive = isActive;
}
