#include <entityx/Entity.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/components.h>

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
	if(entity.has_component<DirectionComponent>())
	{
		DirectionComponent::Handle directionComponent{entity.component<DirectionComponent>()};
		if(entity.has_component<BodyComponent>())
		{
			BodyComponent::Handle bodyComponent{entity.component<BodyComponent>()};
			if(bodyComponent->bodies.find("main") != bodyComponent->bodies.end())
			{
				b2Body* body{bodyComponent->bodies.at("main")};
				b2World* world{body->GetWorld()};

				//Copute a box in front of the character
				b2AABB handToHandBox;
				if(directionComponent->direction == Direction::Left)
				{
					handToHandBox.lowerBound = body->GetWorldCenter() - b2Vec2(-1, -0.8);
					handToHandBox.upperBound = body->GetWorldCenter() + b2Vec2(0, 0.8);
				}
				else
				{
					handToHandBox.lowerBound = body->GetWorldCenter() - b2Vec2(0, -0.8);
					handToHandBox.upperBound = body->GetWorldCenter() + b2Vec2(1, 0.8);
				}
				HandToHandQueryCallback callback(entity);
				world->QueryAABB(&callback, handToHandBox);

				if(callback.foundEntity.valid())
				{
					if(callback.foundEntity.has_component<HealthComponent>())
						callback.foundEntity.component<HealthComponent>()->current -= 1000;
				}
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

HandToHandQueryCallback::HandToHandQueryCallback(entityx::Entity attacker):
	m_attacker(attacker)
{}

bool HandToHandQueryCallback::ReportFixture(b2Fixture* fixture)
{
	entityx::Entity entity{*static_cast<entityx::Entity*>(fixture->GetBody()->GetUserData())};
	//Return false (and so stop) only if this is a arrow and if this one is sticked.
	bool found{entity != m_attacker and entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Actor};
	if(found)
		foundEntity = entity;
	return not found;
}
