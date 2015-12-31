#include <cassert>
#include <algorithm>
#include <functional>
#include <TheLostGirl/functions.h>

template <typename Animation>
const std::function<bool(const typename AnimationsManager<Animation>::TimeAnimation&, const typename AnimationsManager<Animation>::TimeAnimation&)> AnimationsManager<Animation>::m_animationsComparator{
		[](const TimeAnimation& lhs, const TimeAnimation& rhs)
		{
			return not lhs.isActive or ((lhs.importance < rhs.importance) and rhs.isActive);
		}};

template<typename Animation>
void AnimationsManager<Animation>::addAnimation(const std::string& identifier, Animation animation, unsigned int importance, sf::Time duration, bool loops, bool stopAtEnd)
{
	auto it = getAnimation(identifier);
	if(not TEST(it == m_animations.end()))
		return;
	m_animations.emplace_back(TimeAnimation{identifier, animation, importance, duration, loops, stopAtEnd});
	std::push_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
	TimeAnimation& front = m_animations.front();
	if(front.identifier == identifier)
		front.animation.animate(front.progress);
}

template<typename Animation>
void AnimationsManager<Animation>::removeAnimation(const std::string& identifier)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	m_animations.erase(it);
	std::make_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
}

template<typename Animation>
bool AnimationsManager<Animation>::isPaused(const std::string& identifier) const
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.cend()))
		return false;
	return it->isPaused;
}

template<typename Animation>
bool AnimationsManager<Animation>::isActive(const std::string& identifier) const
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.cend()))
		return false;
	return it->isActive;
}

template<typename Animation>
void AnimationsManager<Animation>::play(const std::string& identifier)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	it->isPaused = false;
	it->isActive = true;
	std::make_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
}

template<typename Animation>
void AnimationsManager<Animation>::activate(const std::string& identifier)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	it->isActive = true;
	std::make_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
}

template<typename Animation>
void AnimationsManager<Animation>::pause(const std::string& identifier)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	it->isPaused = true;
}

template<typename Animation>
void AnimationsManager<Animation>::stop(const std::string& identifier)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	stopImpl(*it);
	std::make_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
}

template<typename Animation>
float AnimationsManager<Animation>::getProgress(const std::string& identifier) const
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.cend()))
		return 0.f;
	return it->progress;
}

template<typename Animation>
void AnimationsManager<Animation>::setProgress(const std::string& identifier, float newProgress)
{
	auto it = getAnimation(identifier);
	if(not TEST(it != m_animations.end()))
		return;
	it->progress = newProgress;
}
template<typename Animation>
void AnimationsManager<Animation>::update(sf::Time dt)
{
	if(not m_animations.empty())
	{
		TimeAnimation& timeAnim = m_animations.front();
		//If timeAnim is inactive, all other animations are too
		if(timeAnim.isActive)
		{
			if(not timeAnim.isPaused)
			{
				if(timeAnim.loops)//Always increment progress if the animation loops
				{
					timeAnim.progress += dt.asSeconds()/timeAnim.duration.asSeconds();
					if(timeAnim.progress > 1.f)//If we must loops
						timeAnim.progress -= std::floor(timeAnim.progress);//Keep the progress in the range [0, 1]
				}
				else if(timeAnim.progress < 1.f)//If the animation doesn't loops, incerement progress only if less than 1
				{
					timeAnim.progress += dt.asSeconds()/timeAnim.duration.asSeconds();
					if(timeAnim.progress > 1.f and timeAnim.stopAtEnd)
					{
						stopImpl(timeAnim);
						std::pop_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
						std::push_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
					}
				}
			}
			timeAnim.animation.animate(timeAnim.progress);
		}
	}
}

template<typename Animation>
bool AnimationsManager<Animation>::isRegistred(const std::string& identifier) const
{
	return getAnimation(identifier) != m_animations.cend();
}

template<typename Animation>
Json::Value AnimationsManager<Animation>::serialize() const
{
	Json::Value ret;
	for(auto& animation : m_animations)
	{
		ret[animation.identifier]["importance"] = animation.importance;
		ret[animation.identifier]["duration"] = animation.duration.asSeconds();
		ret[animation.identifier]["loops"] = animation.loops;
		ret[animation.identifier]["stop at end"] = animation.stopAtEnd;
		ret[animation.identifier]["progress"] = animation.progress;
		ret[animation.identifier]["is paused"] = animation.isPaused;
		ret[animation.identifier]["is active"] = animation.isActive;
		ret[animation.identifier]["is active"] = animation.isActive;
		ret[animation.identifier]["data"] = animation.animation.serialize();
	}
	return ret;
}

template<typename Animation>
template<typename T>
void AnimationsManager<Animation>::deserialize(const Json::Value& value, T& object)
{
	m_animations.clear();
	//For each animation name in the value
	for(std::string& animationIdentifier : value.getMemberNames())
	{
		Json::Value animationObj = value[animationIdentifier];
		Animation animation(object);
		//Copy the data inside the animation
		animation.deserialize(animationObj["data"]);
		TimeAnimation timeAnim{animationIdentifier, animation};
		if(animationObj.isMember("importance"))
			timeAnim.importance = animationObj["importance"].asUInt();
		if(animationObj.isMember("duration"))
			timeAnim.duration = sf::seconds(animationObj["duration"].asFloat());
		if(animationObj.isMember("loops"))
			timeAnim.loops = animationObj["loops"].asBool();
		if(animationObj.isMember("progress"))
			timeAnim.progress = animationObj["progress"].asFloat();
		if(animationObj.isMember("is active"))
			timeAnim.isPaused = animationObj["is paused"].asBool();
		if(animationObj.isMember("is active"))
			timeAnim.isActive = animationObj["is active"].asBool();
		if(animationObj.isMember("stop at end"))
			timeAnim.stopAtEnd = animationObj["stop at end"].asBool();
		m_animations.push_back(timeAnim);
		std::push_heap(m_animations.begin(), m_animations.end(), m_animationsComparator);
	}
	m_animations.front().animation.animate(m_animations.front().progress);
}

template<typename Animation>
typename std::vector<typename AnimationsManager<Animation>::TimeAnimation>::iterator AnimationsManager<Animation>::getAnimation(const std::string& identifier)
{
	return std::find_if(m_animations.begin(), m_animations.end(), [&identifier](const TimeAnimation& animation)
	{
		return animation.identifier == identifier;
	});
}

template<typename Animation>
typename std::vector<typename AnimationsManager<Animation>::TimeAnimation>::const_iterator AnimationsManager<Animation>::getAnimation(const std::string& identifier) const
{
	return std::find_if(m_animations.cbegin(), m_animations.cend(), [&identifier](const TimeAnimation& animation)
	{
		return animation.identifier == identifier;
	});
}

template<typename Animation>
inline void AnimationsManager<Animation>::stopImpl(TimeAnimation& animation)
{
	animation.isPaused = true;
	animation.isActive = false;
	animation.progress = 0.f;
}
