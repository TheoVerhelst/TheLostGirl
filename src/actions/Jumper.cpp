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
	auto jumpComponent(entity.component<JumpComponent>());
	const auto fallComponent(entity.component<FallComponent>());
	auto animationsComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
	const auto directionComponent(entity.component<DirectionComponent>());
	if(jumpComponent and fallComponent)
	{
		if(not fallComponent->inAir)
		{
			jumpComponent->mustJump = true;//The physics system will do the physic jump job
			if(animationsComponent and directionComponent)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationsComponent->animationsManager);
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
