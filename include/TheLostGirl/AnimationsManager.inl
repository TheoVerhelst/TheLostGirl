#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include <dist/json/json.h>

template<typename T>
AnimationsManager<T>::AnimationsManager():
	m_animationsMap()
{}

template<typename T>
AnimationsManager<T>::~AnimationsManager()
{
	for(auto& pair : m_animationsMap)
		delete pair.second.animation;
}

template<typename T>
void AnimationsManager<T>::addAnimation(const std::string& identifier,
								Animation<T>* animation,
								unsigned short int importance,
								sf::Time duration,
								bool loop)
{
	TimeAnimation timeAnime{animation, importance, duration, loop};
	m_animationsMap.emplace(identifier, timeAnime);
}

template<typename T>
void AnimationsManager<T>::removeAnimation(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	m_animationsMap.erase(found);
}

template<typename T>
bool AnimationsManager<T>::isPaused(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isPaused;
}

template<typename T>
bool AnimationsManager<T>::isActive(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isActive;
}

template<typename T>
void AnimationsManager<T>::play(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = false;
	found->second.isActive = true;
}

template<typename T>
void AnimationsManager<T>::activate(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isActive = true;
}

template<typename T>
void AnimationsManager<T>::pause(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
}

template<typename T>
void AnimationsManager<T>::stop(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
	found->second.isActive = false;
	found->second.progress = 0.f;
}

template<typename T>
float AnimationsManager<T>::getProgress(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.progress;
}

template<typename T>
void AnimationsManager<T>::setProgress(const std::string& identifier, float newProgress)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.progress = newProgress;
}
template<typename T>
void AnimationsManager<T>::update(T& object, sf::Time dt)
{
	if(not m_animationsMap.empty())
	{
		//Find the animation to play
		typename std::map<std::string, TimeAnimation>::iterator mostImportant;//The map is T-dependent, so the typename keyword is necessary
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
			}
			timeAnim.animation->animate(object, timeAnim.progress);
		}
	}
}

template<typename T>
bool AnimationsManager<T>::isRegistred(const std::string& identifier) const
{
	return m_animationsMap.find(identifier) != m_animationsMap.end();
}

template<typename T>
Json::Value AnimationsManager<T>::serialize() const
{
	Json::Value ret;
	for(auto& pair : m_animationsMap)
	{
		ret[pair.first]["importance"] = pair.second.importance;
		ret[pair.first]["duration"] = pair.second.duration.asSeconds();
		ret[pair.first]["loops"] = pair.second.loops;
		ret[pair.first]["progress"] = pair.second.progress;
		ret[pair.first]["is paused"] = pair.second.isPaused;
		ret[pair.first]["is active"] = pair.second.isActive;
		ret[pair.first]["is active"] = pair.second.isActive;
		ret[pair.first]["data"] = pair.second.animation->serialize();
	}
	return ret;
}

template<typename T>
AnimationsManager<T>::TimeAnimation::TimeAnimation(Animation<T>* _animation, unsigned short int _importance, sf::Time _duration, bool _loops):
	animation{_animation},
	importance{_importance},
	duration{_duration},
	loops{_loops},
	progress{0.f},
	isPaused{true},
	isActive{false}
{}
