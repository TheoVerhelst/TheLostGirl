#ifndef SCRIPTSSYSTEM_H
#define SCRIPTSSYSTEM_H

#include <queue>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}
struct Command;

/// System that plays animations on entities.
/// \see AnimationsManager
class ScriptsSystem : public entityx::System<ScriptsSystem>
{
	public:
		/// Default constructor.
		/// \param commandQueue Queue where push commands.
		ScriptsSystem(std::queue<Command>& commandQueue);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		std::queue<Command>& m_commandQueue;
};

#endif//SCRIPTSSYSTEM_H
