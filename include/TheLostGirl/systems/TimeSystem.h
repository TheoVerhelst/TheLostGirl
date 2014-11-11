#ifndef TIMESYSTEM_H
#define TIMESYSTEM_H

#include <entityx/System.h>

//Forward declarations
namespace entityx
{
	class EventManager;
	class EntityManager;
}

class TimeSystem : public entityx::System<TimeSystem>
{
	public:
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
		void update(entityx::EntityManager&, entityx::EventManager&, double dt);
		void setTotalTime(sf::Time totalTime);
		std::wstring seasonName() const;//Return the name of the current season
		std::wstring monthName() const;//Return the name of the current month
		std::wstring getFormatedDate() const;//Return the date in the format "%seasonName\n%numberYear ème année, %numberDay ème jour du %monthName"
		std::wstring getFormatedTime() const;//Return the time in the format "%hour:%minutes"
		unsigned years() const;//Return the number of years in the non real time of the game
		unsigned months() const;
		unsigned days() const;
		unsigned hours() const;
		unsigned minutes() const;
		sf::Time getRealTime() const;
		float getWindStrength() const;
		
	private:
		float m_totalTime;//Real time spended in game, in seconds
		float m_windStrength;//Strength of the wind, can be positive (from left to right) or negative (from right to left)
		float m_initialWindStrength;//The initial speed of the wind
		float m_nextWindStrength;//The speed that the wind tends to be
		float m_periodBeginning;
		float m_windTransitionLength;//Time spend by the wind to be m_nextWindStrength
		float m_microInitialWindStrength;//Same as higher, but in smaller proportions
		float m_microNextWindStrength;
		float m_microPeriodBeginning;
		float m_microWindTransitionLength;
};

#endif // TIMESYSTEM_H