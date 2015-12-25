#ifndef INVENTORYOPENER_H
#define INVENTORYOPENER_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

class InventoryOpener : public Action
{
	public:
		/// Default constructor.
		InventoryOpener();

		/// Default destructor
		virtual ~InventoryOpener();

		/// Overload of the () operator.
		/// \param entity Entity who do the action.
		/// \param dt Elapsed time in the last game frame.
		virtual void operator()(entityx::Entity entity) const;
};

#endif//INVENTORYOPENER_H

