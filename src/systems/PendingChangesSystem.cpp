#include <algorithm>
#include <cmath>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/Category.hpp>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/Command.hpp>
#include <TheLostGirl/systems/PendingChangesSystem.hpp>

void PendingChangesSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	while(not commandQueue.empty())
	{
		Command& command(commandQueue.front());
		if(command.isTargetSpecific())
			command.getAction()(command.getEntity());
		else
		{
			CategoryComponent::Handle categoryComponent;
			for(auto entity : entityManager.entities_with_components(categoryComponent))
				//On vérifie si l'entité correspond à la commande, si oui on fait l'action
				if((categoryComponent->category & command.getCategory()).any())
					command.getAction()(entity);
		}
		commandQueue.pop();
	}
	while(not bodiesToCreate.empty())
	{
		getWorld().CreateBody(bodiesToCreate.front());
		delete bodiesToCreate.front();//Delete the definition
		bodiesToCreate.pop();
	}
	while(not bodiesToDestroy.empty())
	{
		getWorld().DestroyBody(bodiesToDestroy.front());
		bodiesToDestroy.pop();
	}
	while(not jointsToCreate.empty())
	{
		getWorld().CreateJoint(jointsToCreate.front());
		delete jointsToCreate.front();//Delete the definition
		jointsToCreate.pop();
	}
	while(not jointsToDestroy.empty())
	{
		getWorld().DestroyJoint(jointsToDestroy.front());
		jointsToDestroy.pop();
	}
}
