#include <entityx/entityx.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>

#include <TheLostGirl/systems/StatsSystem.h>

void StatsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, double dt)
{
	HealthComponent::Handle healthComponent;
	StaminaComponent::Handle staminaComponent;
	
	//For every entity that can fall, set the right animation
	for(auto entity : entityManager.entities_with_components(healthComponent))
	{
		if(healthComponent->current < healthComponent->maximum)
		{
			healthComponent->current = cap(healthComponent->current + healthComponent->regeneration*dt, 0.f, healthComponent->maximum);
			eventManager.emit<PlayerHealthChange>(healthComponent->current);
		}
	}
	//For every entity that can fall, set the right animation
	for(auto entity : entityManager.entities_with_components(staminaComponent))
	{
		if(staminaComponent->current < staminaComponent->maximum)
		{
			staminaComponent->current = cap(staminaComponent->current + staminaComponent->regeneration*dt, 0.f, staminaComponent->maximum);
			eventManager.emit<PlayerStaminaChange>(staminaComponent->current);
		}
		if(entity.has_component<BowComponent>())
		{
			staminaComponent->current = cap(staminaComponent->current - entity.component<BowComponent>()->power*dt*3/100.f, 0.f, staminaComponent->maximum);
			eventManager.emit<PlayerStaminaChange>(staminaComponent->current);
		}
	}
}
