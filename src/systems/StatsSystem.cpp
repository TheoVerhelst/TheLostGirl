#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/events.hpp>
#include <TheLostGirl/Command.hpp>
#include <TheLostGirl/actions.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>
#include <TheLostGirl/systems/StatsSystem.hpp>

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
			Context::getSystemManager().system<PendingChangesSystem>()->commandQueue.emplace(entity, Death());

		//Regen health
		if(not isDead and healthComponent->current < healthComponent->maximum)
		{
			healthComponent->current = cap(healthComponent->current + healthComponent->regeneration*float(dt), 0.f, healthComponent->maximum);
			eventManager.emit<EntityHealthChange>(entity, healthComponent->current, healthComponent->current/healthComponent->maximum);
		}
	}

	for(auto entity : entityManager.entities_with_components(staminaComponent))
	{
		const float initialStamina{staminaComponent->current};
		//Substract stamina if the entity bend his bow
		ArticuledArmsComponent::Handle armsComponent(entity.component<ArticuledArmsComponent>());
		if(armsComponent and armsComponent->arms.valid())
		{
			HoldItemComponent::Handle holdItemComponent(armsComponent->arms.component<HoldItemComponent>());
			if(holdItemComponent and holdItemComponent->item.valid())
			{
				const BowComponent::Handle bowComponent(holdItemComponent->item.component<BowComponent>());
				if(bowComponent)
					staminaComponent->current = cap(staminaComponent->current - bowComponent->targetTranslation*float(dt)*3.f/100.f, 0.f, staminaComponent->maximum);
			}
		}

		//Regen stamina
		if(staminaComponent->current < staminaComponent->maximum)
			staminaComponent->current = cap(staminaComponent->current + staminaComponent->regeneration*float(dt), 0.f, staminaComponent->maximum);

		if(std::abs(staminaComponent->current - initialStamina) > 0.001f)
			eventManager.emit<EntityStaminaChange>(entity, staminaComponent->current, staminaComponent->current/staminaComponent->maximum);
	}
}
