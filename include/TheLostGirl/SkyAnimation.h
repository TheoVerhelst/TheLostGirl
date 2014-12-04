#ifndef SKYANIMATION_H
#define SKYANIMATION_H

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Animation of a sky entity.
/// This class rotate and set the transparence of sprites of a given entity
/// to simulate the day/night revolution.
class SkyAnimation
{
	public:
		/// Constructor.
		/// \param entity Entity that represents the sky.
		SkyAnimation(entityx::Entity entity);
		
		/// Apply the animation to the entity passed as argument in the constructor.
		/// \param progress Progress of the animation to apply.
		void animate(float progress);
		
	private:
		entityx::Entity m_entity;///< The entity to animate.
};

#endif//SKYANIMATION_H
