#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2Body.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/systems/StatsSystem.h>

StatsSystem::StatsSystem()
{}

void StatsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, double dt)
{
	HealthComponent::Handle healthComponent;
	StaminaComponent::Handle staminaComponent;

	for(auto entity : entityManager.entities_with_components(healthComponent))
	{
		//Damage all entities that fall in the void
		if(entity.has_component<BodyComponent>())
		{
			for(auto& pair : entity.component<BodyComponent>()->bodies)
			{
				if(pair.second->GetPosition().y > 300)
				{
					healthComponent->current = cap(healthComponent->current - 50*dt, 0.f, healthComponent->maximum);
					if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Player)
						eventManager.emit<PlayerHealthChange>(healthComponent->current, healthComponent->current/healthComponent->maximum);
					break;
				}
			}
		}
		//Regen health
		if(healthComponent->current < healthComponent->maximum)
		{
			healthComponent->current = cap(healthComponent->current + healthComponent->regeneration*dt, 0.f, healthComponent->maximum);
			if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Player)
				eventManager.emit<PlayerHealthChange>(healthComponent->current, healthComponent->current/healthComponent->maximum);
		}
	}

	for(auto entity : entityManager.entities_with_components(staminaComponent))
	{
		//Substract stamina if the entity bend his bow
		if(entity.has_component<BowComponent>())
		{
			float oldStamina{staminaComponent->current};
			staminaComponent->current = cap(staminaComponent->current - entity.component<BowComponent>()->power*dt*3/100.f, 0.f, staminaComponent->maximum);
			if(staminaComponent->current < oldStamina
				and entity.has_component<CategoryComponent>()
				and entity.component<CategoryComponent>()->category & Category::Player)
				eventManager.emit<PlayerStaminaChange>(staminaComponent->current, staminaComponent->current/staminaComponent->maximum);
		}

		//Regen stamina
		if(staminaComponent->current < staminaComponent->maximum)
		{
			staminaComponent->current = cap(staminaComponent->current + staminaComponent->regeneration*dt, 0.f, staminaComponent->maximum);
			if(entity.has_component<CategoryComponent>() and entity.component<CategoryComponent>()->category & Category::Player)
				eventManager.emit<PlayerStaminaChange>(staminaComponent->current, staminaComponent->current/staminaComponent->maximum);
		}
	}
}
