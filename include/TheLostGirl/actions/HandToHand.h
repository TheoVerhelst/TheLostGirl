#ifndef HANDTOHAND_H
#define HANDTOHAND_H

#include <unordered_set>

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.h>
#include <TheLostGirl/HashEntity.h>

/// Performs a hand to hand attack in front on the entity.
/// This attack strikes all entities that are near the entity at once.
class HandToHand : public Action
{
	public:
		/// Constructor.
		HandToHand() = default;

		/// Destructor.
		virtual ~HandToHand() = default;

		/// Overload of the () operator.
		/// \param entity Entity who do the action.
		virtual void operator()(entityx::Entity entity) const override;
};

/// AABB query callback that indicate if some actors are found into the AABB.
class HandToHandQueryCallback : public b2QueryCallback
{
	private:
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

