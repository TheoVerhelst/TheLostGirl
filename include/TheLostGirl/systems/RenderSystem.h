#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <entityx/System.h>
#include <TheLostGirl/StateStack.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}
namespace sf
{
	class RenderWindow;
	class RenderTexture;
}

/// System that draws all drawables entities on the screen.
class RenderSystem : public entityx::System<RenderSystem>
{
	public:
		/// Constructor.
		/// \param context Current context of the application.
		RenderSystem(StateStack::Context context);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;  ///< SFML's window on wich render the entities.
		sf::RenderTexture& m_texture;///< Render texture on wich render the entities, if bloom is enabled.
		bool& m_bloomEnabled;        ///< Indicates if the bloom effect i enabled.
		bool m_postEffectSupported;  ///< Indicates if post effects are supported (avoid useless calls of PostEffect::isSupported())
};

#endif//RENDERSYSTEM_H
