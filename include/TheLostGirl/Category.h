#ifndef CATEGORY_H
#define CATEGORY_H

/// \file Category.h

namespace Category
{
	/// Category of entity.
	/// The category is used to dipatch tasks and commands between differents types of entities.
	enum Category
	{
		None       = 0,      ///< Category that correspond to none category.
		Player     = 1 << 0, ///< Category for the entity controlled by the player.
		Scene      = 1 << 1, ///< Category for every scene entity.
		Passive    = 1 << 2, ///< Category of passive actor.
		Aggressive = 1 << 3, ///< Category of aggressive actor.
	};
}

#endif//SCATEGORY_H
