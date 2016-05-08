#ifndef HASHENTITY_HPP
#define HASHENTITY_HPP

#include <cstddef>

//Forward declarations
namespace entityx
{
	class Entity;
}

/// Hash function object for entities.
/// Hash entities allow to make unordered set of entities,  with the benefit that implies:
/// making a set of entities, with constant time insertion without duplicate,
/// and using entities as key in unordered_map, with constant time lookup.
class HashEntity
{
	public:
		/// Hash fonction.
		/// \param entity The entity to hash.
		/// \return The hashed value, based on the entity's ID.
		std::size_t operator()(entityx::Entity entity) const;
};

#endif//HASHENTITY_HPP
