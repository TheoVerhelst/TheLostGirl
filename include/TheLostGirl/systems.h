#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics/Vertex.hpp>

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
class Command;
typedef std::queue<Command> CommandQueue;
struct Parameters;

/// System that applies actions on the players's entity.
class Actions : public entityx::System<Actions>
{
	public:
		/// Default constructor.
		/// \param commandQueue Queue of command where the actions should be in.
		Actions(CommandQueue& commandQueue):
			m_commandQueue(commandQueue)
		{}

		/// System's update function.
		/// \warning Every call to this function empties the CommandQueue passed as paramter in the constructor.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		CommandQueue& m_commandQueue;///< Queue of command where the actions should be in.
};

/// System that set the correct view according to the player position.
class ScrollingSystem : public entityx::System<ScrollingSystem>
{
	public:
		/// Default constructor.
		/// \param window SFML's window of wich set the view.
		/// \param parameters Structure containing all the game parameters.
		ScrollingSystem(sf::RenderWindow& window, Parameters& parameters):
			m_window(window),
			m_parameters(parameters),
			m_levelRect{0, 0, 0, 0}
		{}
		
		/// Set the level bounds. This must be called before the first update.
		/// \param levelRect Rectangle defining the level bounds.
		/// \param referencePlan Number of the plan where actors evolute.
		void setLevelData(const sf::IntRect& levelRect, float referencePlan);

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		sf::RenderWindow& m_window;///< SFML's window of wich set the view.
		Parameters& m_parameters;  ///< Structure containing all the game parameters.
		sf::IntRect m_levelRect;   ///< Rectangle defining the level bounds.
		float m_referencePlan;     ///< Number of the plan where actors evolute.
};

/// System that handle the sky animation.
class SkySystem : public entityx::System<SkySystem>
{
	public:
		/// Default constructor.
		SkySystem()
		{}

		/// System's update function.
		/// \warning The spentTime argument do not refers to the elapsed time in the last game frame,
		/// but to the total time spend in the game!
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param spentTime Total time spend in the game.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double spentTime) override;
};

/// System that handle the drag and drop.
/// It draw a line on the screen, and set the bending angle of the playr's archer.
class DragAndDropSystem : public entityx::System<DragAndDropSystem>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the drag and drop line.
		/// \param commandQueue Queue of command where the actions should be in.
		DragAndDropSystem(sf::RenderWindow& window, CommandQueue& commandQueue):
			m_window(window),
			m_commandQueue(commandQueue),
			m_origin{0, 0},
			m_line{sf::Vertex({0, 0}, sf::Color::Black),
				   sf::Vertex({0, 0}, sf::Color::Black)},//Initialize the line and set his color
			m_isActive{false}
		{}

		/// System's update function.
		/// This function must be called if the drag and drop is not active.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

		/// Set the activation of the drag and drop.
		/// The line will be drawn only if the drag and drop is active.
		/// \param isActive True if the drag and drop is active, false otherwhise.
		void setDragAndDropActivation(bool isActive);

	private:
		sf::RenderWindow& m_window;      ///< SFML's window on wich to render the entities.
		CommandQueue& m_commandQueue;    ///< Queue of command where the actions should be in.
		sf::Vector2i m_origin;
		sf::Vertex m_line[2];            ///< The drag and drop line.
		bool m_isActive;                 ///< True when the drag and drop is actived.
};

/// System that draws all drawables entities on the screen.
class Render : public entityx::System<Render>
{
	public:
		/// Default constructor.
		/// \param window SFML's window on wich to render the entities.
		Render(sf::RenderWindow& window):
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

/// System that updates the Box2D's world and the drawables entities.
class Physics : public entityx::System<Physics>
{
	public:
		/// Default constructor.
		/// \param world b2World where the physics entities should be in.
		/// \param parameters Structure containing all the game parameters.
		Physics(b2World& world, Parameters& parameters):
			m_world(world),
			m_parameters(parameters)
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;

	private:
		b2World& m_world;        ///< b2World where the physics entities should be in.
		Parameters& m_parameters;///< Structure containing all the game parameters.
};

/// System that plays animations on entities.
/// \see Animations
class AnimationSystem : public entityx::System<AnimationSystem>
{
	public:
		/// Default constructor.
		AnimationSystem()
		{}

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
};

/// System that manage the gameplay time and wind.
class TimeSystem : public entityx::System<TimeSystem>
{
	public:
		/// Default constructor.
		/// \param beginTime Time with wich to begin the TimeSystem.
		TimeSystem(sf::Time beginTime = sf::seconds(0.f)) :
			m_totalTime{beginTime.asSeconds()},
			m_windStrength{0.f},
			m_initialWindStrength{0.f},
			m_nextWindStrength{0.f},
			m_periodBeginning{0.f},
			m_windTransitionLength{0.f},
			m_microInitialWindStrength{0.f},
			m_microNextWindStrength{0.f},
			m_microPeriodBeginning{0.f},
			m_microWindTransitionLength{0.f}
		{
			m_periodBeginning = m_totalTime;
			m_initialWindStrength = rand() % 21 - 10; //A random number, between -10 and 10
			m_windTransitionLength = rand() % 586 + 15; //between 15 and 600
			m_nextWindStrength = rand() % 21 - 10; //between -10 and 10
			m_windStrength = m_initialWindStrength;
		}
		
		/// Destructor.
		~TimeSystem();

		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager &es, entityx::EventManager &events, double dt) override;
		
		/// Return the name of the current season.
		/// \return a wide string with the name of the current season.
		std::wstring seasonName() const;
		
		/// Return the name of the current month.
		/// \return a wide string with the name of the current month.
		std::wstring monthName() const;
		
		/// Return the date in the format "%seasonName\n%numberYear ème année, %numberDay ème jour du %monthName".
		/// \return a wide string with the formated date.
		std::wstring getFormatedDate() const;
		
		/// Return the time in the format "%hour:%minutes".
		/// \return a wide string with the formated time.
		std::wstring getFormatedTime() const;
		
		/// Return the number of years in the gameplay time system.
		/// \return The current year.
		unsigned years() const;
		
		/// Return the number of months in the gameplay time system.
		/// \return The number of months in the current year.
		unsigned months() const;
		
		/// Return the number of days in the gameplay time system.
		/// \return The day in the current month.
		unsigned days() const;
		
		/// Return the number of hours in the gameplay time system.
		/// \return The current hour.
		unsigned hours() const;
		
		/// Return the number of minutes in the gameplay time system.
		/// \return The current minute.
		unsigned minutes() const;
		
		/// Get the total time spended in the game.
		/// That is equal to the time since the construction of this instance,
		/// plus the offset gived to the contructor in parameter.
		/// \return The real time.
		sf::Time getRealTime() const;
		
		/// Return the strength of the wind, it can be positive (from left to right) or negative (from right to left).
		/// \return The strength of the wind.
		float getWindStrength() const;
		
	private:
		float m_totalTime;                ///< Real time spended in game, in seconds.
		float m_windStrength;             ///< Strength of the wind, can be positive (from left to right) or negative (from right to left).
		float m_initialWindStrength;      ///< The initial speed of the wind.
		float m_nextWindStrength;         ///< The speed that the wind tends to be.
		float m_periodBeginning;          ///< Value of m_totalTime when the period started.
		float m_windTransitionLength;     ///< Time spend by the wind to be m_nextWindStrength.
		float m_microInitialWindStrength; ///< Same as higher, but in smaller proportions.
		float m_microNextWindStrength;    ///< Same as higher, but in smaller proportions.
		float m_microPeriodBeginning;     ///< Same as higher, but in smaller proportions.
		float m_microWindTransitionLength;///< Same as higher, but in smaller proportions.
};

#endif // SYSTEMS_H