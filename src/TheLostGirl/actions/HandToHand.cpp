#include <algorithm>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/Box2DHelper.hpp>
#include <TheLostGirl/actions/HandToHand.hpp>

void HandToHand::operator()(entityx::Entity entity) const
{
	if(not TEST(entity.valid()))
		return;
	HandToHandComponent::Handle handToHandComponent(entity.component<HandToHandComponent>());
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	if(not TEST(handToHandComponent and directionComponent and bodyComponent))
		return;
	const WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
	if(not walkComponent or walkComponent->effectiveMovement == Direction::None)
	{
		handToHandComponent->lastShoot += handToHandComponent->timer.restart();
		if(handToHandComponent->lastShoot >= handToHandComponent->delay)
		{
			handToHandComponent->lastShoot = sf::Time::Zero;
			b2Body* body{bodyComponent->body};

			//Compute a box in front of the character; first, compute the transform of the box
			b2Transform bodyTransform;
			if(directionComponent->direction == Direction::Left)
				bodyTransform = b2Transform(body->GetPosition() + b2Vec2(-0.5f, 0), b2Rot(body->GetAngle()));
			else
				bodyTransform = b2Transform(body->GetPosition() + b2Vec2(0.5f, 0), b2Rot(body->GetAngle()));
				
			//Then get the AABB of the main fixture and apply the transform
			b2AABB handToHandBox;
			for(b2Fixture* fixture{body->GetFixtureList()}; fixture; fixture = fixture->GetNext())
				if(fixtureHasRole(fixture, FixtureRole::Main))
					fixture->GetShape()->ComputeAABB(&handToHandBox, bodyTransform, 0);
					
			//Do the query
			std::set<entityx::Entity> foundEntities{Box2DHelper::queryEntities(handToHandBox,
				[&entity](entityx::Entity otherEntity)
				{
					return isHittableEntity(otherEntity, entity);
				})};

			for(auto foundEntity : foundEntities)
			{
				HealthComponent::Handle healthComponent(foundEntity.component<HealthComponent>());
				const ActorComponent::Handle actorComponent(foundEntity.component<ActorComponent>());
				if(healthComponent)
				{
					float damages{handToHandComponent->damages};
					damages -= actorComponent->handToHandResistance;
					healthComponent->current -= damages;
				}
			}

			AnimationsComponent<SpriteSheetAnimation>::Handle animationComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
			if(animationComponent)
			{
				AnimationsManager<SpriteSheetAnimation>& animations(animationComponent->animationsManager);
				//If the animations manager have the required animations
				if(animations.isRegistred("hand to hand left") and animations.isRegistred("hand to hand right"))
				{
					if(directionComponent->direction == Direction::Left)
						animations.play("hand to hand left");
					else if(directionComponent->direction == Direction::Right)
						animations.play("hand to hand right");
				}
			}
		}
	}
}

bool HandToHand::isHittableEntity(entityx::Entity entity, entityx::Entity attacker)
{
	//The entity is added to the list only if:
	// -this is not the attacker itself
	// -this is an actor
	// -attacker and current entity are not both aggressive
	const ActorComponent::Handle actorComponent(entity.component<ActorComponent>());
	const CategoryComponent::Handle entityCategoryComponent(entity.component<CategoryComponent>());
	const auto attackerCategoryComponent(attacker.component<CategoryComponent>());
	return entity != attacker and actorComponent
			and not (attackerCategoryComponent and attackerCategoryComponent->category.test(Category::Aggressive)
			and entityCategoryComponent and entityCategoryComponent->category.test(Category::Aggressive));
}
