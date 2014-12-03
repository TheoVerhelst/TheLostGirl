#ifndef ACTIONSSYSTEM_H
#define ACTIONSSYSTEM_H

#include <queue>

#include <SFML/Graphics/Vertex.hpp>

#include <TheLostGirl/Command.h>

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
struct Parameters;

/// System that applies actions on the players's entity.
class ActionsSystem : public entityx::System<ActionsSystem>
{
	public:
		/// Default constructor.
		/// \param commandQueue Queue of command where the actions should be putted in.
		ActionsSystem(std::queue<Command>& commandQueue):
			m_commandQueue(commandQueue)
		{}

		/// System's update function.
		/// \warning Every call to this function empties the std::queue<Command> passed as paramter in the constructor.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		std::queue<Command>& m_commandQueue;///< Queue of command where the actions should be putted in.
};

#endif // ACTIONSSYSTEM_H
