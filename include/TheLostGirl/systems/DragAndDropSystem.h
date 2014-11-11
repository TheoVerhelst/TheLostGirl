#ifndef DRAGANDDROPSYSTEM_H
#define DRAGANDDROPSYSTEM_H

#include <SFML/Graphics/Vertex.hpp>
#include <entityx/System.h>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
}
struct Parameters;

/// System that handle the drag and drop.
/// It draw a line on the screen, and set the bending angle of the playr's archer.
class DragAndDropSystem : public entityx::System<DragAndDropSystem>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the drag and drop line.
		/// \param commandQueue Queue of command where the actions should be in.
		DragAndDropSystem(sf::RenderWindow& window, CommandQueue& commandQueue):
			m_window(window),
			m_commandQueue(commandQueue),
			m_origin{0, 0},
			m_line{sf::Vertex({0, 0}, sf::Color::Black),
				   sf::Vertex({0, 0}, sf::Color::Black)},//Initialize the line and set his color
			m_isActive{false}
		{}
		
		/// System's update function.
		/// This function must be called if the drag and drop is not active.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
		
		/// Set the activation of the drag and drop.
		/// The line will be drawn only if the drag and drop is active.
		/// \param isActive True if the drag and drop is active, false otherwhise.
		void setDragAndDropActivation(bool isActive);
		
	private:
		sf::RenderWindow& m_window;      ///< SFML's window on wich to render the entities.
		CommandQueue& m_commandQueue;    ///< Queue of command where the actions should be in.
		sf::Vector2i m_origin;
		sf::Vertex m_line[2];            ///< The drag and drop line.
		bool m_isActive;                 ///< True when the drag and drop is actived.
};

#endif // DRAGANDDROPSYSTEM_H