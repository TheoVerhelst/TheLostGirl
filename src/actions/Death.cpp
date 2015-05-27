#include <entityx/Entity.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/scripts/scriptsFunctions.h>
#include <TheLostGirl/actions/Death.h>

Death::Death(StateStack::Context context):
	m_context(context)
{
}

Death::~Death()
{
}

void Death::operator()(entityx::Entity entity, double) const
{
	if(not entity)
		return;
	//Play death animation.
	if(entity.has_component<AnimationsComponent<SpriteSheetAnimation>>())
		for(auto& animationsPair : entity.component<AnimationsComponent<SpriteSheetAnimation>>()->animationsManagers)
			if(animationsPair.second.isRegistred("death"))
				animationsPair.second.play("death");
	//Set the death component
	entity.component<DeathComponent>()->dead = true;
	if(entity.has_component<InventoryComponent>())
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
					entityx::Entity item = entity.getManager()->create();
					ItemComponent::Handle itemComponent = item.assign<ItemComponent>();
					itemComponent->category = drop.category;
					itemComponent->type = drop.type;
					entity.component<InventoryComponent>()->items.push_back(item);
				}
			}
		}
	}

	//Send a stop command
	stop({entity}, m_context);
}
