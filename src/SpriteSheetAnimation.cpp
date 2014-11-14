#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <entityx/Entity.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

#include <TheLostGirl/SpriteSheetAnimation.h>

SpriteSheetAnimation::SpriteSheetAnimation(sf::Sprite& sprite):
	m_sprite(sprite)
{}

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
				m_sprite.setTextureRect(frame.rect);
				return;
			}
		}
		m_sprite.setTextureRect(m_frames[m_frames.size()-1].rect);
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

void SpriteSheetAnimation::deserialize(const Json::Value& value, float scale)
{
	m_frames.clear();
	for(size_t i{0}; i < value.size(); ++i)
	{
		sf::IntRect rect;
		float duration;
		rect.left = float(value[i]["x"].asInt())*scale;
		rect.top = float(value[i]["y"].asInt())*scale;
		rect.width = float(value[i]["w"].asInt())*scale;
		rect.height = float(value[i]["h"].asInt())*scale;
		duration = value[i]["relative duration"].asFloat();
		std::cout << "left = " << rect.left << std::endl;
		std::cout << "top = " << rect.top << std::endl;
		std::cout << "width = " << rect.width << std::endl;
		std::cout << "height = " << rect.height << std::endl;
		m_frames.push_back(Frame(rect, duration));
	}
}
