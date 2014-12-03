#ifndef PENDINGCHANGESSYSTEM_H
#define PENDINGCHANGESSYSTEM_H

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

/// System that do things in the pending list (create bodies, destroy joints, aply actions, ...)
class PendingChangesSystem : public entityx::System<PendingChangesSystem>
{
	public:
		/// Default constructor.
		/// \param pendingChanges The set of all pending changes.
		PendingChangesSystem(PendingChanges& pendingChanges, b2World& world):
			m_pendingChanges(pendingChanges),
			m_world(world)
		{}

		/// System's update function.
		/// \warning Every call to this function empties the std::queue<Command> passed as paramter in the constructor.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		PendingChanges& m_pendingChanges;///< The set of all pending changes.
		b2World& m_world;
};

#endif // PENDINGCHANGESSYSTEM_H
