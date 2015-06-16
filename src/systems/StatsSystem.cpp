#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2Body.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/events.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/actions.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>
#include <TheLostGirl/systems/StatsSystem.h>

StatsSystem::StatsSystem(StateStack::Context context):
	m_context(context),
	m_commandQueue(context.systemManager.system<PendingChangesSystem>()->commandQueue)
{}

void StatsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, double dt)
{
	HealthComponent::Handle healthComponent;
	StaminaComponent::Handle staminaComponent;

	for(auto entity : entityManager.entities_with_components(healthComponent))
	{
		//Damage all entities that fall in the void
		const BodyComponent::Handle bodyComponent(entity.component<BodyComponent>());
		if(bodyComponent and bodyComponent->body->GetPosition().y > 300)
		{
			healthComponent->current = cap(healthComponent->current - 50.f*float(dt), 0.f, healthComponent->maximum);
			eventManager.emit<EntityHealthChange>(entity, healthComponent->current, healthComponent->current/healthComponent->maximum);
		}
		const DeathComponent::Handle deathComponent(entity.component<DeathComponent>());
		bool isDead{deathComponent and deathComponent->dead};
		if(not isPlayer(entity) and healthComponent->current <= 0.f and not isDead)
		{
			Command deathCommand;
			deathCommand.targetIsSpecific = true;
			deathCommand.entity = entity;
			deathCommand.action = Death(m_context);
			m_context.systemManager.system<PendingChangesSystem>()->commandQueue.push(deathCommand);
		}

		//Regen health
		if(not isDead and healthComponent->current < healthComponent->maximum)
		{
			healthComponent->current = cap(healthComponent->current + healthComponent->regeneration*float(dt), 0.f, healthComponent->maximum);
			eventManager.emit<EntityHealthChange>(entity, healthComponent->current, healthComponent->current/healthComponent->maximum);
		}
	}

	for(auto entity : entityManager.entities_with_components(staminaComponent))
	{
		//Substract stamina if the entity bend his bow
		const ArcherComponent::Handle bowComponent(entity.component<ArcherComponent>());
		if(bowComponent)
		{
			float oldStamina{staminaComponent->current};
			staminaComponent->current = cap(staminaComponent->current - bowComponent->power*float(dt)*3.f/100.f, 0.f, staminaComponent->maximum);
			if(staminaComponent->current < oldStamina)
				eventManager.emit<EntityStaminaChange>(entity, staminaComponent->current, staminaComponent->current/staminaComponent->maximum);
		}

		//Regen stamina
		if(staminaComponent->current < staminaComponent->maximum)
		{
			staminaComponent->current = cap(staminaComponent->current + staminaComponent->regeneration*float(dt), 0.f, staminaComponent->maximum);
			eventManager.emit<EntityStaminaChange>(entity, staminaComponent->current, staminaComponent->current/staminaComponent->maximum);
		}
	}
}
