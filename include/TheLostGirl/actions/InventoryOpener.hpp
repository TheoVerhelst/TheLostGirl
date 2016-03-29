#ifndef INVENTORYOPENER_HPP
#define INVENTORYOPENER_HPP

#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/Context.hpp>

namespace entityx
{
    class Entity;
}

/// Display a GUI menu that allow to open the inventory of the player.
class InventoryOpener : public Action, private ContextAccessor<ContextElement::StateStack>
{
	public:
		/// Default constructor.
		InventoryOpener();

		/// Default destructor
		virtual ~InventoryOpener();

		/// Overload of the () operator.
		/// \param entity Entity that open its inventory.
		virtual void operator()(entityx::Entity entity) const override;
};

#endif//INVENTORYOPENER_HPP
