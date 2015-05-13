#include <entityx/Entity.h>

#include <TheLostGirl/components.h>

#include <TheLostGirl/actions/Jumper.h>

Jumper::Jumper()
{
}

Jumper::~Jumper()
{
}

void Jumper::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	if(entity.has_component<JumpComponent>() and entity.has_component<FallComponent>())
	{
		FallComponent::Handle fallComponent{entity.component<FallComponent>()};
		if(not fallComponent->inAir)
		{
			entity.component<JumpComponent>()->mustJump = true;//The physics system will do the physic jump job
			if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>()
				and entity.has_component<DirectionComponent>())
			{
				DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
				//Get all the animations managers of the entity
				auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
				//For each animations manager of the entity
				for(auto& animationsPair : animationsManagers)
				{
					AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
					//If the animations manager have the required animations
					if(animations.isRegistred("jump left") and animations.isRegistred("jump right"))
					{
						if(directionComponent->direction == Direction::Left)
							animations.play("jump left");
						else if(directionComponent->direction == Direction::Right)
							animations.play("jump right");
					}
				}
			}
		}
	}
}
