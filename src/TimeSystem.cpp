#include <cmath>
#include <fstream>
#include <cstdlib>

#include <TheLostGirl/LangManager.h>

#include <TheLostGirl/TimeSystem.h>

using namespace std;

TimeSystem::TimeSystem(sf::Time beginTime) :
	m_totalTime(beginTime.asSeconds()),
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

TimeSystem::~TimeSystem()
{}

wstring TimeSystem::seasonName() const
{
	switch(months() / 4)
	{
		case 0:
			return LangManager::tr("Snow season");
			break;
		case 1:
			return LangManager::tr("Flowers season");
			break;
		case 2:
			return LangManager::tr("Fruit season");
			break;
		case 3:
			return LangManager::tr("Wind season");
			break;
		default:
			return LangManager::tr("Snow season");
	}
}

wstring TimeSystem::monthName() const
{
	switch(months())
	{
		case 0:
			return LangManager::tr("snow month");
			break;
		case 1:
			return LangManager::tr("winds month");
			break;
		case 2:
			return LangManager::tr("buds month");
			break;
		case 3:
			return LangManager::tr("flowers month");
			break;
		case 4:
			return LangManager::tr("harvests month");
			break;
		case 5:
			return LangManager::tr("fruits month");
			break;
		case 6:
			return LangManager::tr("mists month");
			break;
		case 7:
			return LangManager::tr("month of frost");
			break;
		default:
			return LangManager::tr("snow month");
	}
}

wstring TimeSystem::getFormatedDate() const
{
	wstring output;
	//Season name
	output += seasonName() + L"\n";
	//Year name
	output += to_wstring(years() + 1);
	if(years() + 1 == 1)
		output += LangManager::tr("st year, ");
	else if(years() + 1 == 2)
		output += LangManager::tr("nd year, ");
	else
		output += LangManager::tr("th year, ");
	//Day name
	output += to_wstring(days() + 1);
	if(days() + 1 == 1)
		output += LangManager::tr("st day of the ");
	else if(days() + 1 == 2)
		output += LangManager::tr("nd day of the ");
	else
		output += LangManager::tr("th day of the ");
	//Month name
	output += monthName();
	return output;
}

wstring TimeSystem::getFormatedTime() const
{
	return to_wstring(hours()) + L":" + to_wstring(minutes());
}

void TimeSystem::update(sf::Time dt)
{
	m_totalTime += dt.asSeconds();
	float seasonFactor = sin((m_totalTime / 24000.f) * 360.f); //Vary between -1  and 1, maximum in the spring and the automn
	seasonFactor = (seasonFactor + 1.f) / 2.f; //Now between 0 and 1

	/*
	The wind works by period of a random number of seconds, m_windTransitionLength,
	and choose a new random number, m_nextWindStrength, at every end of period,
	So the old m_nextWindStrength become m_initialWindStrength,
	and the wind tends to be progressively equal to m_nextWindStrength.
	The wind is also influenced by windStrengthSeasonFactor.
	This system is applied a second time, in smaller proportions.
	*/
	if(m_totalTime >= m_periodBeginning + m_windTransitionLength)//End of a period
	{
		m_periodBeginning = m_totalTime;
		m_initialWindStrength = m_nextWindStrength;
		m_windTransitionLength = static_cast<float>(rand() % 586000 + 15000) / 1000.f; //A random number between 15 and 600
		m_nextWindStrength = static_cast<float>(rand() % 21000 - 10000) / 1000.f; //Another random number, between -10 and 10
	}

	float deltaStrength = m_nextWindStrength - m_initialWindStrength;
	float elapsedTimeSincePeriodBeginning = m_totalTime - m_periodBeginning;
	m_windStrength = m_initialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_windTransitionLength);

	if(m_totalTime >= m_microPeriodBeginning + m_microWindTransitionLength)//End of a period
	{
		m_microPeriodBeginning = m_totalTime;
		m_microInitialWindStrength = m_microNextWindStrength;
		m_microWindTransitionLength = static_cast<float>(rand() % 1600 + 500) / 5000.f; //A random number between 0.5 and 2
		m_microNextWindStrength = static_cast<float>(rand() % 1000 - 2000) / 1000.f; //Another random number, between -1 and 1
	}

	deltaStrength = m_microNextWindStrength - m_microInitialWindStrength;
	elapsedTimeSincePeriodBeginning = m_totalTime - m_microPeriodBeginning;
	m_windStrength += m_microInitialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_microWindTransitionLength);
	m_windStrength *= seasonFactor;//Apply the factor of the season
}

unsigned TimeSystem::years() const
{
	return unsigned(m_totalTime) / 48000;
}

unsigned TimeSystem::months() const
{
	return (unsigned(m_totalTime) % 48000) / 6000;
}

unsigned TimeSystem::days() const
{
	return (unsigned(m_totalTime) % 6000) / 600;
}

unsigned TimeSystem::hours() const
{
	return (unsigned(m_totalTime) % 600) / 30;
}

unsigned TimeSystem::minutes() const
{
	return (unsigned(m_totalTime * 2) % 60);
}

sf::Time TimeSystem::getRealTime() const
{
	return sf::seconds(m_totalTime);
}

float TimeSystem::getWindStrength() const
{
	return m_windStrength;
}
