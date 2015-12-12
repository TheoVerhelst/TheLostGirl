#include <entityx/Entity.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/states/OpenInventoryState.h>

#include <TheLostGirl/actions/InventoryOpener.h>

InventoryOpener::InventoryOpener()
{
}

InventoryOpener::~InventoryOpener()
{
}

void InventoryOpener::operator()(entityx::Entity entity) const
{
	Context::stateStack->pushState<OpenInventoryState>(entity);
}
