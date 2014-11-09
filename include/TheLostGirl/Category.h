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
		Ground  = 1 << 1, ///< Category for every ground entity.
		Scene   = 1 << 2 ///< Category for every scene entity.
	};
}

#endif // CATEGORY_H
