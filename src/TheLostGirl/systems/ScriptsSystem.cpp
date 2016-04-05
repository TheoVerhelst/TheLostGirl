#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/scripts/Interpreter.hpp>
#include <TheLostGirl/ResourceManager.hpp>
#include <TheLostGirl/systems/ScriptsSystem.hpp>

void ScriptsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	ScriptsComponent::Handle scriptsComponent;
	DeathComponent::Handle deadComponent;
	for(entityx::Entity entity : entityManager.entities_with_components(scriptsComponent, deadComponent))
		if(not deadComponent->dead)
			for(auto& scriptName : scriptsComponent->scriptsNames)
				Context::getScriptManager().get(scriptName).interpret(entity);
}
