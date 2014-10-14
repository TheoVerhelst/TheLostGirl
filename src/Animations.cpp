#include <cmath>

#include <Animations.h>

Animations::Animations():
	m_animationsMap(),
	m_isPlaying{false},
	m_progress{0.f},
	m_currentAnimation("")
{
}

Animations::~Animations()
{}

Animations::Animations(const Animations& other)
{
	//copy ctor
}

Animations& Animations::operator=(const Animations& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	//assignment operator
	return *this;
}

void Animations::addAnimation(const std::string& identifier,
							  Animation animation,
							  sf::Time duration,
							  bool loop
							  )
{
	TimeAnimation timeAnime(animation, duration, loop);
	m_animationsMap.emplace(identifier, timeAnime);
}

void Animations::removeAnimation(const std::string& identifier)
{
	m_animationsMap.erase(identifier);
}

bool Animations::isPlaying() const
{
	return m_isPlaying;
}

void Animations::play()
{
	m_isPlaying = true;
}

void Animations::play(std::string animation)
{
	setCurrentAnimation(animation);
	play();
}

void Animations::stop()
{
	m_isPlaying = false;
}

float Animations::getProgress() const
{
	return m_progress;
}

void Animations::setProgress(float newProgress)
{
	m_progress = newProgress - std::floor(newProgress);
}

const std::string& Animations::getCurrentAnimation() const
{
	return m_currentAnimation;
}

void Animations::setCurrentAnimation(std::string animation)
{
	m_currentAnimation = animation;
	m_progress = 0.f;
}

void Animations::update(entityx::Entity& entity, sf::Time dt)
{
	//If we must play an animation and if the current animation is in the map
	if(isPlaying() and m_animationsMap.find(m_currentAnimation) != m_animationsMap.end())
	{
		TimeAnimation& timeAnim = m_animationsMap.at(m_currentAnimation);//Access this element is safe
		m_progress += dt.asSeconds()/timeAnim.duration.asSeconds();
		if(timeAnim.loops and m_progress > 1.f)
			m_progress -= std::floor(m_progress);//Keep the progress in the range [0, 1]
		
		if(m_progress > 1.f)
			stop();
		else
			timeAnim.animation(entity, m_progress);
	}
}
Animations::TimeAnimation::TimeAnimation(Animation _animation, sf::Time _duration, bool _loops):
	animation(_animation),
	duration(_duration),
	loops(_loops)
{}