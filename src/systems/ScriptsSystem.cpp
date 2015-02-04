#include <entityx/entityx.h>

#include <TheLostGirl/components.h>
#include <TheLostGirl/scripts/Interpreter.h>

#include <TheLostGirl/systems/ScriptsSystem.h>

ScriptsSystem::ScriptsSystem()
{
}

void ScriptsSystem::update(entityx::EntityManager& entityManager, entityx::EventManager&, double)
{
	ScriptsComponent::Handle scriptsComponent;
	for(entityx::Entity entity : entityManager.entities_with_components(scriptsComponent))
	{
		for(auto& scriptPair : scriptsComponent->scripts)
		{
			//Rewind the file
			scriptPair.second->clear();
			scriptPair.second->seekg(0);
			Interpreter interpreter(*scriptPair.second, entity);
		    interpreter.interpret();
		}
	}
}
