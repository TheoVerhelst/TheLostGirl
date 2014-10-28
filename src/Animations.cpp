#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include <Animations.h>

Animations::Animations():
	m_animationsMap()
{}

Animations::~Animations()
{}

void Animations::addAnimation(const std::string& identifier,
							Animation animation,
							unsigned short int importance,
							sf::Time duration,
							bool loop)
{
	TimeAnimation timeAnime(animation, importance, duration, loop);
	m_animationsMap.emplace(identifier, timeAnime);
}

void Animations::removeAnimation(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	m_animationsMap.erase(found);
}

bool Animations::isPaused(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isPaused;
}

bool Animations::isActive(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isActive;
}

void Animations::play(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = false;
	found->second.isActive = true;
}

void Animations::activate(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isActive = true;
}

void Animations::pause(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
}

void Animations::stop(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
	found->second.isActive = false;
	found->second.progress = 0.f;
}

float Animations::getProgress(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.progress;
}

void Animations::setProgress(const std::string& identifier, float newProgress)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.progress = newProgress - std::floor(newProgress);
}
void Animations::update(entityx::Entity& entity, sf::Time dt)
{
	if(not m_animationsMap.empty())
	{
		//Find the animation to play
		std::map<std::string, TimeAnimation>::iterator mostImportant;
		bool activeAnimationFound{false};
		for(auto it = m_animationsMap.begin(); it != m_animationsMap.end(); it++)
		{
			if((not activeAnimationFound and it->second.isActive) or
				(activeAnimationFound and it->second.importance > mostImportant->second.importance))
			{
				activeAnimationFound = true;
				mostImportant = it;
			}
		}
		if(activeAnimationFound)
		{
			TimeAnimation& timeAnim = mostImportant->second;
			if(not isPaused(mostImportant->first))
			{
				if(timeAnim.loops)//Always increment progress if the animation loops
				{
					timeAnim.progress += dt.asSeconds()/timeAnim.duration.asSeconds();
					if(timeAnim.progress > 1.f)//If we must loop
						timeAnim.progress -= std::floor(timeAnim.progress);//Keep the progress in the range [0, 1]
				}
				else if(timeAnim.progress < 1.f)//If the animation doesn't loops, incerement progress only if less than 1
					timeAnim.progress += dt.asSeconds()/timeAnim.duration.asSeconds();
				timeAnim.animation(entity, timeAnim.progress);
			}
		}
	}
}
Animations::TimeAnimation::TimeAnimation(Animation _animation, unsigned short int _importance, sf::Time _duration, bool _loops):
	animation(_animation),
	importance(_importance),
	duration(_duration),
	loops(_loops),
	progress(0.f),
	isPaused(true),
	isActive(false)
{}