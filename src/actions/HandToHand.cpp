#include <algorithm>

#include <entityx/Entity.h>
#include <Box2D/Dynamics/b2World.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/FixtureRoles.h>

#include <TheLostGirl/actions/HandToHand.h>

HandToHand::HandToHand()
{
}

HandToHand::~HandToHand()
{
}

void HandToHand::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	if(entity.has_component<DirectionComponent>() and entity.has_component<HandToHandComponent>()
		and (not entity.has_component<WalkComponent>() or entity.component<WalkComponent>()->effectiveMovement == Direction::None))
	{
		HandToHandComponent::Handle handToHandComponent{entity.component<HandToHandComponent>()};
		handToHandComponent->lastShoot += handToHandComponent->timer.restart();
		if(handToHandComponent->lastShoot >= handToHandComponent->delay)
		{
			handToHandComponent->lastShoot = sf::Time::Zero;
			DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
			if(entity.has_component<BodyComponent>())
			{
				BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
				if(bodyComponent->bodies.find("main") != bodyComponent->bodies.end())
				{
					b2Body* body{bodyComponent->bodies.at("main")};
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
						if(foundEntity.valid() and foundEntity.has_component<HealthComponent>())
							foundEntity.component<HealthComponent>()->current -= entity.component<HandToHandComponent>()->damages;
				}
			}
			if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>())
			{
				//Get all the animations managers of the entity
				auto& animationsManagers(entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers);
				//For each animations manager of the entity
				for(auto& animationsPair : animationsManagers)
				{
					AnimationsManager<SpriteSheetAnimation>& animations(animationsPair.second);
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
	if(entity != m_attacker and entity.has_component<ActorComponent>()
			and not (m_attacker.has_component<CategoryComponent>()
				and m_attacker.component<CategoryComponent>()->category & Category::Aggressive
				and entity.component<CategoryComponent>()->category & Category::Aggressive))
		foundEntities.insert(entity);
	return true;
}
