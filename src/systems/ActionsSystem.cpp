#include <algorithm>
#include <cmath>
#include <iostream>

#include <entityx/entityx.h>

#include <TheLostGirl/Category.h>
#include <TheLostGirl/components.h>

#include <TheLostGirl/systems/ActionsSystem.h>

void ActionsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double dt)
{
	while(not m_commandQueue.empty())
	{
		Command& command = m_commandQueue.front();
		CategoryComponent::Handle categoryComponent;
		if(command.targetIsSpecific)
			command.action(*(command.entity), dt);
		else
			for(auto entity : entityManager.entities_with_components(categoryComponent))
				//On vérifie si l'entité correspond à la commande, si oui on fait l'action
				if(categoryComponent->category & command.category)
					command.action(entity, dt);
		m_commandQueue.pop();
	}
}
