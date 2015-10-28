#include <algorithm>
#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2World.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/actions/HandToHand.h>

void HandToHand::operator()(entityx::Entity entity) const
{
	if(not entity)
		return;
	HandToHandComponent::Handle handToHandComponent(entity.component<HandToHandComponent>());
	const DirectionComponent::Handle directionComponent(entity.component<DirectionComponent>());
	const WalkComponent::Handle walkComponent(entity.component<WalkComponent>());
	BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
	if(directionComponent and handToHandComponent and bodyComponent
		and (not walkComponent or walkComponent->effectiveMovement == Direction::None))
	{
		handToHandComponent->lastShoot += handToHandComponent->timer.restart();
		if(handToHandComponent->lastShoot >= handToHandComponent->delay)
		{
			handToHandComponent->lastShoot = sf::Time::Zero;
			b2Body* body{bodyComponent->body};
			b2World* world{body->GetWorld()};

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
			HandToHandQueryCallback callback(entity);
			world->QueryAABB(&callback, handToHandBox);

			for(entityx::Entity foundEntity : callback.foundEntities)
			{
				if(foundEntity.valid())
				{
					HealthComponent::Handle healthComponent(foundEntity.component<HealthComponent>());
					const ActorComponent::Handle actorComponent(foundEntity.component<ActorComponent>());
					if(healthComponent and actorComponent)
					{
						float damages{handToHandComponent->damages};
						damages -= actorComponent->handToHandResistance;
						healthComponent->current -= damages;
					}
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

HandToHandQueryCallback::HandToHandQueryCallback(entityx::Entity attacker):
	m_attacker(attacker)
{}

bool HandToHandQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	//The entity is added to the list only if:
	// -this is not the attacker itself
	// -this is an actor
	// -attacker and current entity are not both aggressive
	const ActorComponent::Handle actorComponent(entity.component<ActorComponent>());
	const CategoryComponent::Handle entityCategoryComponent(entity.component<CategoryComponent>());
	const auto attackerCategoryComponent(m_attacker.component<CategoryComponent>());
	if(entity != m_attacker and actorComponent
			and not (attackerCategoryComponent and attackerCategoryComponent->category & Category::Aggressive
				and entityCategoryComponent and entityCategoryComponent->category & Category::Aggressive))
		foundEntities.insert(entity);
	return true;
}
