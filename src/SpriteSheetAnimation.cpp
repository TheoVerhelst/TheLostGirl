#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/SpriteSheetAnimation.h>

SpriteSheetAnimation::SpriteSheetAnimation()
{}

SpriteSheetAnimation::SpriteSheetAnimation(const SpriteSheetAnimation& other):
	m_frames(other.m_frames)
{}

SpriteSheetAnimation& SpriteSheetAnimation::operator=(const SpriteSheetAnimation& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	return *this;
}

void SpriteSheetAnimation::addFrame(const Frame& frame)
{
	m_frames.push_back(frame);
}

void SpriteSheetAnimation::addFrame(const sf::IntRect& rect, float duration)
{
	m_frames.push_back(Frame(rect, duration));
}

void SpriteSheetAnimation::animate(sf::Sprite& sprite, float progress)
{
	if(not m_frames.empty())
	{
		//Find the current frame
		float progressCounter{0};
		for(auto& frame : m_frames)
		{
			
			progressCounter += frame.duration;
			if(progressCounter > progress)
			{
				sprite.setTextureRect(frame.rect);
				return;
			}
		}
		sprite.setTextureRect(m_frames[m_frames.size()-1].rect);
	}
}

Json::Value SpriteSheetAnimation::serialize() const
{
	Json::Value ret;
	for(size_t i{0}; i < m_frames.size(); ++i)
	{
		ret[i]["x"] = m_frames[i].rect.left;
		ret[i]["y"] = m_frames[i].rect.top;
		ret[i]["w"] = m_frames[i].rect.width;
		ret[i]["h"] = m_frames[i].rect.height;
		ret[i]["relative duration"] = m_frames[i].duration;
	}
	return ret;
}
