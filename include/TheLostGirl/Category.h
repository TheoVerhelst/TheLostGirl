#ifndef CATEGORY_H
#define CATEGORY_H

/// \file Category.h

namespace Category
{
	/// Category of entity.
	/// The category is used to dipatch tasks and commands between differents types of entities.
	enum Category
	{
		None    = 0,      ///< Category that correspond to none category.
		Player  = 1 << 0, ///< Category for the entity controlled by the player.
		Scene   = 1 << 1, ///< Category for every scene entity.
		Actor   = 1 << 2  ///< Category for every actor entity (mobs, pnj, etc... but not scene entities or objects).
	};
}

#endif // CATEGORY_H
