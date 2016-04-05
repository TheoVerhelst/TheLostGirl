#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/actions/Jumper.hpp>

void Jumper::operator()(entityx::Entity entity) const
{
	if(not TEST(entity.valid()))
		return;
	JumpComponent::Handle jumpComponent(entity.component<JumpComponent>());
	const FallComponent::Handle fallComponent(entity.component<FallComponent>());
	if(not TEST(jumpComponent and fallComponent))
		return;
	AnimationsComponent<SpriteSheetAnimation>::Handle animationsComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
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
