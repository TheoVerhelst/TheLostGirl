#include <cmath>
#include <SFML/System/Time.hpp>

#include <TheLostGirl/LangManager.h>
#include <TheLostGirl/systems/TimeSystem.h>

TimeSystem::TimeSystem(StateStack::Context context):
	m_totalTime{0.f},
	m_windStrength{0.f},
	m_initialWindStrength{0.f},
	m_nextWindStrength{0.f},
	m_periodBeginning{0.f},
	m_windTransitionLength{0.f},
	m_microInitialWindStrength{0.f},
	m_microNextWindStrength{0.f},
	m_microPeriodBeginning{0.f},
	m_microWindTransitionLength{0.f},
	m_rd{},
	m_gen{m_rd()},
	m_transitionLenghtDis{15, 600},
	m_strengthDis{-2, 2},
	m_langManager(context.langManager)
{
	m_periodBeginning = m_totalTime;
	m_initialWindStrength = float(rand() % 21) - 10.f; //A random number, between -10 and 10
	m_windTransitionLength = float(rand() % 586) + 15.f; //between 15 and 600
	m_nextWindStrength = float(rand() % 21) - 10.f; //between -10 and 10
	m_windStrength = m_initialWindStrength;
}

void TimeSystem::update(entityx::EntityManager&, entityx::EventManager&, double dt)
{
	m_totalTime += float(dt);
	float seasonFactor{std::sin((m_totalTime / 24000.f) * 360.f)}; //Vary between -1  and 1, maximum in the spring and the automn
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
		m_windTransitionLength = m_transitionLenghtDis(m_gen);//A random number between 15 and 600
		m_nextWindStrength = m_strengthDis(m_gen);//Another random number, between -2 and 2
	}

	float deltaStrength{m_nextWindStrength - m_initialWindStrength};
	float elapsedTimeSincePeriodBeginning{m_totalTime - m_periodBeginning};
	m_windStrength = m_initialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_windTransitionLength);

	if(m_totalTime >= m_microPeriodBeginning + m_microWindTransitionLength)//End of a period
	{
		m_microPeriodBeginning = m_totalTime;
		m_microInitialWindStrength = m_microNextWindStrength;
		std::random_device rd;
		std::mt19937 gen{rd()};
		std::uniform_real_distribution<float> transitionLenghtDis(0.5f, 2.f);
		m_microWindTransitionLength = transitionLenghtDis(gen);//A random number between 0.5 and 2
		std::uniform_real_distribution<float> strengthDis(-0.2f, 0.2f);
		m_microNextWindStrength = strengthDis(gen);//Another random number, between -1 and 1
	}

	deltaStrength = m_microNextWindStrength - m_microInitialWindStrength;
	elapsedTimeSincePeriodBeginning = m_totalTime - m_microPeriodBeginning;
	m_windStrength += m_microInitialWindStrength + (deltaStrength * elapsedTimeSincePeriodBeginning / m_microWindTransitionLength);
	m_windStrength *= seasonFactor;//Apply the factor of the season
}

void TimeSystem::setTotalTime(sf::Time totalTime)
{
	m_totalTime = totalTime.asSeconds();
}

std::wstring TimeSystem::seasonName() const
{
	switch(months() / 4)
	{
		case 0:
			return m_langManager.tr("Snow season");
			break;
		case 1:
			return m_langManager.tr("Flowers season");
			break;
		case 2:
			return m_langManager.tr("Fruit season");
			break;
		case 3:
			return m_langManager.tr("Wind season");
			break;
		default:
			return m_langManager.tr("Snow season");
	}
}

std::wstring TimeSystem::monthName() const
{
	switch(months())
	{
		case 0:
			return m_langManager.tr("snow month");
			break;
		case 1:
			return m_langManager.tr("winds month");
			break;
		case 2:
			return m_langManager.tr("buds month");
			break;
		case 3:
			return m_langManager.tr("flowers month");
			break;
		case 4:
			return m_langManager.tr("harvests month");
			break;
		case 5:
			return m_langManager.tr("fruits month");
			break;
		case 6:
			return m_langManager.tr("mists month");
			break;
		case 7:
			return m_langManager.tr("month of frost");
			break;
		default:
			return m_langManager.tr("snow month");
	}
}

std::wstring TimeSystem::getFormatedDate() const
{
	std::wstring output;
	//Season name
	output += seasonName() + L"\n";
	//Year name
	output += std::to_wstring(years() + 1);
	if(years() + 1 == 1)
		output += m_langManager.tr("st year, ");
	else if(years() + 1 == 2)
		output += m_langManager.tr("nd year, ");
	else
		output += m_langManager.tr("th year, ");
	//Day name
	output += std::to_wstring(days() + 1);
	if(days() + 1 == 1)
		output += m_langManager.tr("st day of the ");
	else if(days() + 1 == 2)
		output += m_langManager.tr("nd day of the ");
	else
		output += m_langManager.tr("th day of the ");
	//Month name
	output += monthName();
	return output;
}

std::wstring TimeSystem::getFormatedTime() const
{
	return std::to_wstring(hours()) + L":" + std::to_wstring(minutes());
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
