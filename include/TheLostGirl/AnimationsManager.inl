#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include <dist/json/json.h>

#include <TheLostGirl/SpriteSheetAnimation.h>

template<typename A>
AnimationsManager<A>::AnimationsManager():
	m_animationsMap()
{}

template<typename A>
AnimationsManager<A>::~AnimationsManager()
{}

template<typename A>
void AnimationsManager<A>::addAnimation(const std::string& identifier, A animation, unsigned short int importance, sf::Time duration, bool loop)
{
	TimeAnimation timeAnime{animation, importance, duration, loop};
	m_animationsMap.emplace(identifier, timeAnime);
}

template<typename A>
void AnimationsManager<A>::removeAnimation(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	m_animationsMap.erase(found);
}

template<typename A>
bool AnimationsManager<A>::isPaused(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isPaused;
}

template<typename A>
bool AnimationsManager<A>::isActive(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.isActive;
}

template<typename A>
void AnimationsManager<A>::play(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = false;
	found->second.isActive = true;
}

template<typename A>
void AnimationsManager<A>::activate(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isActive = true;
}

template<typename A>
void AnimationsManager<A>::pause(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
}

template<typename A>
void AnimationsManager<A>::stop(const std::string& identifier)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.isPaused = true;
	found->second.isActive = false;
	found->second.progress = 0.f;
}

template<typename A>
float AnimationsManager<A>::getProgress(const std::string& identifier) const
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	return found->second.progress;
}

template<typename A>
void AnimationsManager<A>::setProgress(const std::string& identifier, float newProgress)
{
	auto found = m_animationsMap.find(identifier);
	assert(found != m_animationsMap.end());//Assert that the animation exists
	found->second.progress = newProgress;
}
template<typename A>
void AnimationsManager<A>::update(sf::Time dt)
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
			timeAnim.animation.animate(timeAnim.progress);
		}
	}
}

template<typename A>
bool AnimationsManager<A>::isRegistred(const std::string& identifier) const
{
	return m_animationsMap.find(identifier) != m_animationsMap.end();
}

template<typename A>
Json::Value AnimationsManager<A>::serialize() const
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
		ret[pair.first]["data"] = pair.second.animation.serialize();
	}
	return ret;
}

template<typename A> template<typename T>
void AnimationsManager<A>::deserialize(const Json::Value& value, T& object, float scale)
{
	m_animationsMap.clear();
	//For each animation name in the value
	for(std::string& animationName : value.getMemberNames())
	{
		Json::Value animationObj = value[animationName];
		A animation(object);
		//Copy the data inside the animation
		animation.deserialize(animationObj["data"], scale);
		TimeAnimation timeAnim(animation);
		timeAnim.importance = animationObj["importance"].asUInt();
		timeAnim.duration = sf::seconds(animationObj["duration"].asFloat());
		timeAnim.loops = animationObj["loops"].asBool();
		timeAnim.progress = animationObj["progress"].asFloat();
		timeAnim.isPaused = animationObj["is paused"].asBool();
		timeAnim.isActive = animationObj["is active"].asBool();
		m_animationsMap.emplace(animationName, timeAnim);
	}
}

template<typename A>
AnimationsManager<A>::TimeAnimation::TimeAnimation(A _animation, unsigned short int _importance, sf::Time _duration, bool _loops, float _progress, bool _isPaused, bool _isActive):
	animation{_animation},
	importance{_importance},
	duration{_duration},
	loops{_loops},
	progress{_progress},
	isPaused{_isPaused},
	isActive{_isActive}
{}
