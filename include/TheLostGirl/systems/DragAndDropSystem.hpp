#ifndef DRAGANDDROPSYSTEM_HPP
#define DRAGANDDROPSYSTEM_HPP

#include <queue>
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>

/// System that handle the drag and drop.
/// It draw a line on the screen, and set the bending angle of the playr's archer.
class DragAndDropSystem : public entityx::System<DragAndDropSystem>
{
	public:
		/// Constructor.
		DragAndDropSystem();

		/// System's update function.
		/// This function must be called if the drag and drop is not active.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

		/// Set the activation of the drag and drop.
		/// The line will be drawn only if the drag and drop is active.
		/// \param isActive True if the drag and drop is active, false otherwise.
		void setDragAndDropActivation(bool isActive);

	private:
		sf::Vector2i m_origin;              ///< Position of the mouse when the drag and drop started.
		sf::Vertex m_line[2];               ///< The drag and drop line.
		bool m_isActive;                    ///< True when the drag and drop is actived.
};

#endif//DRAGANDDROPSYSTEM_HPP
