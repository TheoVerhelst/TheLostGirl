#ifndef TIMESYSTEM_H
#define TIMESYSTEM_H

#include <string>

#include <SFML/System/Time.hpp>

/// System that manage the gameplay time and wind.
class TimeSystem
{
	public:
		/// Default constructor.
		/// \param beginTime Time with wich to begin the TimeSystem.
		TimeSystem(sf::Time beginTime = sf::seconds(0));
		
		/// Destructor.
		~TimeSystem();
		
		/// Update the TimeSystem.
		/// Call this member every game tick.
		/// \param dt Elapsed time in the last game frame.
		void update(sf::Time dt);
		
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

#endif // TIMESYSTEM_H
