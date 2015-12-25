#ifndef CATEGORY_H
#define CATEGORY_H

/// \file Category.h

/// Category of entity.
/// The category is used to dipatch tasks and commands between differents types of entities.
enum class Category : std::size_t
{
	None,      ///< Category that correspond to none category.
	Player,    ///< Category for the entity controlled by the player.
	Scene,     ///< Category for every scene entity.
	Passive,   ///< Category of passive actor.
	Aggressive,///< Category of aggressive actor.
	MaxValue   ///< Sentinel value for using FlagSet.
};

#endif//SCATEGORY_H
