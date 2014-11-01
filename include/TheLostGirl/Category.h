#ifndef CATEGORY_H
#define CATEGORY_H

/// \file Category.h

namespace Category
{
	/// Category of entity.
	/// The category is used to dipatch tasks and commands between differents types of entities.
	enum Category
	{
		None    = 0,      ///< Category that correspond to nothing.
		Player  = 1 << 0, ///< Category for the entity controlled by the player.
		Ground  = 1 << 1, ///< Category of the ground.
		CanFall = 1 << 2  ///< Category for every entity that can fall in the void.
	};
}

#endif // CATEGORY_H
