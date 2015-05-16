#ifndef HANDTOHAND_H
#define HANDTOHAND_H

#include <unordered_set>

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Performs a hand to hand attack in front on the entity.
/// This attack strikes all entities that are near the entity at once.
struct HandToHand : public Action
{
	/// Constructor.
	HandToHand();

	/// Destructor.
	virtual ~HandToHand();

	/// Overload of the () operator.
	/// \param entity Entity who do the action.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;

	private:
};

/// AABB query callback that indicate if some actors are found into the AABB.
class HandToHandQueryCallback : public b2QueryCallback
{
	private:
		/// Hash function object for entities.
		/// Hash entities allow to make unordered set of entities,  with the benefit that implies:
		/// making a set of entities, with constant time insertion without duplicate.
		struct HashEntity
		{
			/// Hash fonction.
			/// \param entity The entity to hash.
			/// \return The hashed value, based on the entity's ID.
			size_t operator()(entityx::Entity entity) const;
		};
		entityx::Entity m_attacker;///< The entity that performs the attack (and then excluded from the result).

	public:
		/// Constructor.
		/// \param attacker The entity that does the attack.
		HandToHandQueryCallback(entityx::Entity attacker);

		/// Handle the world querying.
		/// \param fixture A fixture that overlap the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		std::unordered_set<entityx::Entity, HashEntity> foundEntities;///< Set of all entities that overlaps the AABB.
};

#endif//HANDTOHAND_H

