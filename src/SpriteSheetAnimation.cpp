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

void SpriteSheetAnimation::operator()(entityx::Entity& entity, float progress)
{
	if(entity.has_component<SpriteComponent>())
	{
		SpriteComponent::Handle spriteComponent = entity.component<SpriteComponent>();
		//Find the current frame
		float progressCounter{0};
		for(auto& frame : m_frames)
		{
			
			progressCounter += frame.duration;
			if(progressCounter > progress)
			{
				spriteComponent->sprite->setTextureRect(frame.rect);
				return;
			}
		}
		spriteComponent->sprite->setTextureRect(m_frames[m_frames.size()-1].rect);
	}
}