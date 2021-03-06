#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>

SpriteSheetAnimation::SpriteSheetAnimation(sf::Sprite& sprite):
	m_sprite(&sprite)
{
}

void SpriteSheetAnimation::addFrame(const Frame& frame)
{
	m_frames.push_back(frame);
}

void SpriteSheetAnimation::addFrame(const sf::IntRect& rect, float duration)
{
	m_frames.push_back(Frame(rect, duration));
}

void SpriteSheetAnimation::animate(float progress)
{
	if(not m_frames.empty())
	{
		//Find the current frame
		float progressCounter{0};
		for(auto& frame : m_frames)
		{

			progressCounter += frame.duration;
			if(progressCounter >= progress)
			{
				m_sprite->setTextureRect(frame.rect);
				return;
			}
		}
		m_sprite->setTextureRect(m_frames[m_frames.size()-1].rect);
	}
}

Json::Value SpriteSheetAnimation::serialize() const
{
	Json::Value ret;
	for(unsigned int i{0}; i < m_frames.size(); ++i)
	{
		ret[i]["x"] = m_frames[i].rect.left;
		ret[i]["y"] = m_frames[i].rect.top;
		ret[i]["w"] = m_frames[i].rect.width;
		ret[i]["h"] = m_frames[i].rect.height;
		ret[i]["relative duration"] = m_frames[i].duration;
	}
	return ret;
}

void SpriteSheetAnimation::deserialize(const Json::Value& value)
{
	m_frames.clear();
	for(unsigned int i{0}; i < value.size(); ++i)
	{
		sf::IntRect rect;
		float duration;
		rect.left = value[i]["x"].asInt();
		rect.top = value[i]["y"].asInt();
		rect.width = value[i]["w"].asInt();
		rect.height = value[i]["h"].asInt();
		duration = value[i]["relative duration"].asFloat();
		m_frames.push_back(Frame(rect, duration));
	}
}
