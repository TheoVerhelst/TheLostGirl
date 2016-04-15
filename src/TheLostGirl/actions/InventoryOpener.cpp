#include <entityx/entityx.h>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/states/OpenInventoryState.hpp>
#include <TheLostGirl/actions/InventoryOpener.hpp>

void InventoryOpener::operator()(entityx::Entity entity) const
{
	Context::getStateStack().pushState<OpenInventoryState>(entity);
}
