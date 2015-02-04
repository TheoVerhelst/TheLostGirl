#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/SpriteSheetAnimation.h>

SpriteSheetAnimation::SpriteSheetAnimation(sf::Sprite& sprite, StateStack::Context context) :
	m_sprite(sprite),
	m_context(context)
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
			if(progressCounter > progress)
			{
				m_sprite.setTextureRect(scale<int>(frame.rect));
				return;
			}
		}
		m_sprite.setTextureRect(scale<int>(m_frames[m_frames.size()-1].rect));
	}
}

Json::Value SpriteSheetAnimation::serialize() const
{
	Json::Value ret;
	for(unsigned int i{0}; i < m_frames.size(); ++i)
	{
		ret[i]["x"] = int(float(m_frames[i].rect.left));
		ret[i]["y"] = int(float(m_frames[i].rect.top));
		ret[i]["w"] = int(float(m_frames[i].rect.width));
		ret[i]["h"] = int(float(m_frames[i].rect.height));
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
		rect.left = float(value[i]["x"].asInt());
		rect.top = float(value[i]["y"].asInt());
		rect.width = float(value[i]["w"].asInt());
		rect.height = float(value[i]["h"].asInt());
		duration = value[i]["relative duration"].asFloat();
		m_frames.push_back(Frame(rect, duration));
	}
}
