#include <algorithm>
#include <cmath>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems/PendingChangesSystem.h>

void PendingChangesSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	while(not commandQueue.empty())
	{
		Command& command(commandQueue.front());
		CategoryComponent::Handle categoryComponent;
		if(command.targetIsSpecific)
			command.action(command.entity);
		else
			for(auto entity : entityManager.entities_with_components(categoryComponent))
				//On vérifie si l'entité correspond à la commande, si oui on fait l'action
				if(categoryComponent->category & command.category)
					command.action(entity);
		commandQueue.pop();
	}
	while(not bodiesToCreate.empty())
	{
		Context::world->CreateBody(bodiesToCreate.front());
		delete bodiesToCreate.front();//Delete the definition
		bodiesToCreate.pop();
	}
	while(not bodiesToDestroy.empty())
	{
		Context::world->DestroyBody(bodiesToDestroy.front());
		bodiesToDestroy.pop();
	}
	while(not jointsToCreate.empty())
	{
		Context::world->CreateJoint(jointsToCreate.front());
		delete jointsToCreate.front();//Delete the definition
		jointsToCreate.pop();
	}
	while(not jointsToDestroy.empty())
	{
		Context::world->DestroyJoint(jointsToDestroy.front());
		jointsToDestroy.pop();
	}
}
