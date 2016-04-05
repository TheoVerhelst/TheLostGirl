#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/states/OpenInventoryState.hpp>

#include <TheLostGirl/actions/InventoryOpener.hpp>

InventoryOpener::InventoryOpener()
{
}

InventoryOpener::~InventoryOpener()
{
}

void InventoryOpener::operator()(entityx::Entity entity) const
{
	Context::getStateStack().pushState<OpenInventoryState>(entity);
}
