#ifndef SkYSYSTEM_H
#define SkYSYSTEM_H

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that handle the sky animation.
class SkySystem : public entityx::System<SkySystem>
{
	public:
		/// Default constructor.
		SkySystem()
		{}
		
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

#endif // SkYSYSTEM_H