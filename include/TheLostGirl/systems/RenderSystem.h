#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

//Forward declarations
namespace sf
{
	class RenderWindow;
}
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
		RenderSystem(sf::RenderWindow& window):
			m_window(window)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;///< SFML's window on wich to render the entities.
};

#endif//RENDERSYSTEM_H
