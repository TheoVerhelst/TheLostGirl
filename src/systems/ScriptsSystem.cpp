#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/scripts/Interpreter.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/systems/ScriptsSystem.h>

void ScriptsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	ScriptsComponent::Handle scriptsComponent;
	DeathComponent::Handle deadComponent;
	for(entityx::Entity entity : entityManager.entities_with_components(scriptsComponent, deadComponent))
	{
		if(not deadComponent->dead)
			for(auto& scriptName : scriptsComponent->scriptsNames)
				Context::scriptManager->get(scriptName).interpret(entity);
	}
}
