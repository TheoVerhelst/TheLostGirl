#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <algorithm>

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
		/// \param dt Current frame elapsed time.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
 
	private:
		CommandQueue& m_commandQueue;///< Queue of command where the actions should be in.
};

/// System that draws all drawables entities on the screen.
class Render : public entityx::System<Render>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich render the entities.
		Render(sf::RenderWindow& window):
			m_window(window)
		{}
		
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Current frame elapsed time.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;///< SFML's window on wich render the entities.
};

/// System that updates the b2Box's world and the drawables entities.
class Physics : public entityx::System<Physics>
{
	public:
		/// Default constructor.
		/// \param world b2World where the physics entities should be in.
		Physics(b2World& world):
			m_world(world)
		{}
		
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Current frame elapsed time.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;///< b2World where the physics entities should be in.
};

#endif // SYSTEMS_H