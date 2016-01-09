#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <entityx/entityx.h>

/// System that draws all drawables entities on the screen.
class RenderSystem : public entityx::System<RenderSystem>
{
	public:
		/// Constructor.
		RenderSystem();

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		const bool m_postEffectSupported;  ///< Indicates if post effects are supported (avoid useless calls of PostEffect::isSupported())
};

#endif//RENDERSYSTEM_HPP