#ifndef SCRIPTSSYSTEM_H
#define SCRIPTSSYSTEM_H

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that plays animations on entities.
/// \see AnimationsManager
class ScriptsSystem : public entityx::System<ScriptsSystem>
{
	public:
		/// Default constructor.
		ScriptsSystem();

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif//SCRIPTSSYSTEM_H
