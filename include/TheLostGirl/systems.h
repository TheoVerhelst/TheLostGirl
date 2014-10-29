#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics/Vertex.hpp>

//Forward declarations
namespace sf
{
	class RenderWindow;
}
namespace entityx
{
	class EventManager;
	class EntityManager;
	class SystemManager;
}
class b2World;
class Command;
typedef std::queue<Command> CommandQueue;
struct Parameters;

/// System that applies actions on the players's entity.
class Actions : public entityx::System<Actions>
{
	public:
		/// Default constructor.
		/// \param commandQueue Queue of command where the actions should be in.
		Actions(CommandQueue& commandQueue):
			m_commandQueue(commandQueue)
		{}

		/// System's update function.
		/// \warning Every call to this function empties the CommandQueue passed as paramter in the constructor.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		CommandQueue& m_commandQueue;///< Queue of command where the actions should be in.
};

/// System that handle the jump animation.
class FallSystem : public entityx::System<FallSystem>
{
	public:
		/// Default constructor.
		FallSystem()
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

/// System that handle the bow bending animation.
class BendSystem : public entityx::System<BendSystem>
{
	public:
		/// Default constructor.
		BendSystem()
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

/// System that handle the drag and drop.
/// It draw a line on the screen, and set the bending angle of the playr's archer.
class DragAndDropSystem : public entityx::System<DragAndDropSystem>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the drag and drop line.
		DragAndDropSystem(sf::RenderWindow& window):
			m_window(window),
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
		sf::Vertex m_line[2];            ///< The drag and drop line.
		bool m_isActive;                 ///< True when the drag and drop is actived.
};

/// System that draws all drawables entities on the screen.
class Render : public entityx::System<Render>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the entities.
		Render(sf::RenderWindow& window):
			m_window(window)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;///< SFML's window on wich to render the entities.
};

/// System that updates the Box2D's world and the drawables entities.
class Physics : public entityx::System<Physics>
{
	public:
		/// Default constructor.
		/// \param world b2World where the physics entities should be in.
		Physics(b2World& world, Parameters& parameters):
			m_world(world),
			m_parameters(parameters)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;        ///< b2World where the physics entities should be in.
		Parameters& m_parameters;///< Structure containing all the game parameters.
};

/// System that plays animations on entities.
/// \see Animations
class AnimationSystem : public entityx::System<AnimationSystem>
{
	public:
		/// Default constructor.
		AnimationSystem()
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};
#endif // SYSTEMS_H