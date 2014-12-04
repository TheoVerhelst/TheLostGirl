#ifndef TIMESYSTEM_H
#define TIMESYSTEM_H

#include <entityx/System.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

/// System that manages the in-game date and time.
class TimeSystem : public entityx::System<TimeSystem>
{
	public:
		/// Default constructor.
		TimeSystem():
			m_totalTime(0),
			m_windStrength(0),
			m_initialWindStrength(0),
			m_nextWindStrength(0),
			m_periodBeginning(0),
			m_windTransitionLength(0),
			m_microInitialWindStrength(0),
			m_microNextWindStrength(0),
			m_microPeriodBeginning(0),
			m_microWindTransitionLength(0)
		{
			m_periodBeginning = m_totalTime;
			m_initialWindStrength = rand() % 21 - 10; //A random number, between -10 and 10
			m_windTransitionLength = rand() % 586 + 15; //between 15 and 600
			m_nextWindStrength = rand() % 21 - 10; //between -10 and 10
			m_windStrength = m_initialWindStrength;
		}
		
		/// System's update function.
		/// \param es Entity manager.
		/// \param events Event manager.
		/// \param dt Elapsed time in the last game frame.
		void update(entityx::EntityManager& es, entityx::EventManager& events, double dt);
		
		/// Set the date.
		/// \param totalTime New date.
		void setTotalTime(sf::Time totalTime);
		
		/// Return the name of the current season.
		/// \return The name of the current season.
		std::wstring seasonName() const;
		
		/// Return the name of the current month.
		/// \return The name of the current month.
		std::wstring monthName() const;
		
		/// Return the date in the format "%seasonName\n%numberYear ème année, %numberDay ème jour du %monthName" (without quotes).
		/// \return The current date.
		std::wstring getFormatedDate() const;
		
		/// Return the time in the format "%hour:%minutes".
		/// \return The current time.
		std::wstring getFormatedTime() const;
		
		/// Return the number of years in game measure.
		/// \return The current year.
		unsigned years() const;
		
		/// Return the number of months in game measure.
		/// \return The current month.
		unsigned months() const;
		
		/// Return the number of days in game measure.
		/// \return The current day.
		unsigned days() const;
		
		/// Return the number of hours in game measure.
		/// \return The current hour.
		unsigned hours() const;
		
		/// Return the number of minutes in game measure.
		/// \return The current minute.
		unsigned minutes() const;
		
		/// Return the real time spended in the game.
		/// \return The total time.
		sf::Time getRealTime() const;
		
		/// Return the current wind strength.
		/// \return The wind strength.
		float getWindStrength() const;
		
	private:
		float m_totalTime;                ///< Real time spended in game, in seconds.
		float m_windStrength;             ///< Strength of the wind, can be positive (from left to right) or negative (from right to left).
		float m_initialWindStrength;      ///< The initial speed of the wind.
		float m_nextWindStrength;         ///< The speed that the wind tends to be.
		float m_periodBeginning;          ///< The value of m_totalTime at the beginning of the period.
		float m_windTransitionLength;     ///< Time spend by the wind to be m_nextWindStrength.
		float m_microInitialWindStrength; ///< Same as higher, but in smaller proportions.
		float m_microNextWindStrength;    ///< Same as higher, but in smaller proportions.
		float m_microPeriodBeginning;     ///< Same as higher, but in smaller proportions.
		float m_microWindTransitionLength;///< Same as higher, but in smaller proportions.
};

#endif//TIMESYSTEM_H
