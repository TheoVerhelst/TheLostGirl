#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <TheLostGirl/BloomEffect.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that draws all drawables entities on the screen.
class RenderSystem : public entityx::System<RenderSystem>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the entities.
		RenderSystem(sf::RenderWindow& window, sf::RenderTexture& texture, Parameters& parameters):
			m_window(window),
			m_texture(texture),
			m_bloomEffect(),
			m_sprite(m_texture.getTexture()),
			m_parameters(parameters)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;  ///< SFML's window on wich to render the entities.
		sf::RenderTexture& m_texture;///< A render texture to apply the shaders.
		BloomEffect m_bloomEffect;   ///< The shader.
		sf::Sprite m_sprite;         ///< The sprite to display the main texture.
		Parameters& m_parameters;      ///< The current application parameters.
};

#endif//RENDERSYSTEM_H
