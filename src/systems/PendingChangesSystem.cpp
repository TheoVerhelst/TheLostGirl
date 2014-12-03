#include <algorithm>
#include <cmath>
#include <iostream>

#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>

#include <TheLostGirl/Category.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/PendingChanges.h>

#include <TheLostGirl/systems/PendingChangesSystem.h>

void PendingChangesSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	while(not m_pendingChanges.commandQueue.empty())
	{
		Command& command(m_pendingChanges.commandQueue.front());
		CategoryComponent::Handle categoryComponent;
		if(command.targetIsSpecific)
			command.action(*(command.entity), dt);
		else
			for(auto entity : entityManager.entities_with_components(categoryComponent))
				//On vérifie si l'entité correspond à la commande, si oui on fait l'action
				if(categoryComponent->category & command.category)
					command.action(entity, dt);
		m_pendingChanges.commandQueue.pop();
	}
	while(not m_pendingChanges.bodiesToCreate.empty())
	{
		m_world.CreateBody(m_pendingChanges.bodiesToCreate.front());
		m_pendingChanges.bodiesToCreate.pop();
	}
	while(not m_pendingChanges.bodiesToDestroy.empty())
	{
		m_world.DestroyBody(m_pendingChanges.bodiesToDestroy.front());
		m_pendingChanges.bodiesToDestroy.pop();
	}
	while(not m_pendingChanges.jointsToCreate.empty())
	{
		m_world.CreateJoint(m_pendingChanges.jointsToCreate.front());
		m_pendingChanges.jointsToCreate.pop();
	}
	while(not m_pendingChanges.jointsToDestroy.empty())
	{
		m_world.DestroyJoint(m_pendingChanges.jointsToDestroy.front());
		m_pendingChanges.jointsToDestroy.pop();
	}
}
