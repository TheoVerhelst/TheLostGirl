#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>

/// System that draws all drawables entities on the screen.
class RenderSystem : public entityx::System<RenderSystem>, private ContextAccessor<ContextElement::Window,
                                                                                   ContextElement::Parameters,
                                                                                   ContextElement::PostEffectsTexture>
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
