#include <random>
#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <TheLostGirl/SpriteSheetAnimation.hpp>
#include <TheLostGirl/scripts/scriptsFunctions.hpp>
#include <TheLostGirl/actions/Death.hpp>

void Death::operator()(entityx::Entity entity) const
{
	if(not TEST(entity))
		return;
	//Play death animation.
	AnimationsComponent<SpriteSheetAnimation>::Handle spriteSheetComponent(entity.component<AnimationsComponent<SpriteSheetAnimation>>());
	if(spriteSheetComponent)
		if(spriteSheetComponent->animationsManager.isRegistred("death"))
			spriteSheetComponent->animationsManager.play("death");
	//Set the death component
	entity.component<DeathComponent>()->dead = true;
	InventoryComponent::Handle inventoryComponent(entity.component<InventoryComponent>());
	if(inventoryComponent)
	{
		//Drop items
		std::random_device randomDevice;
		std::mt19937 generator{randomDevice()};
		std::uniform_real_distribution<float> distribution{0, 1};
		for(auto& drop : entity.component<DeathComponent>()->drops)
		{
			float dice;
			for(unsigned int i{0}; i < drop.maxDrops; ++i)
			{
				dice = distribution(generator);
				if(dice <= drop.probability)
				{
					//Construct and place item into the entity inventory.
					entityx::Entity item = Context::getEntityManager().create();
					ItemComponent::Handle itemComponent = item.assign<ItemComponent>();
					itemComponent->category = drop.category;
					itemComponent->type = drop.type;
					itemComponent->weight = drop.weight;
					itemComponent->value = drop.value;
					inventoryComponent->items.push_back(item);
				}
			}
		}
	}
	else
		std::cerr << "An entity dead but has not inventory." << std::endl;
	//Send a stop command
	ScriptFunctions::stop({entity});
}
