#ifndef CORPSESEARCHER_H
#define CORPSESEARCHER_H

#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <TheLostGirl/Action.h>
#include <TheLostGirl/StateStack.h>

namespace entityx
{
    class Entity;
}

/// Structure that search on a corpse around the entity.
struct CorpseSearcher : public Action
{
	/// Destructor.
	virtual ~CorpseSearcher() = default;

	/// Overload of the () operator.
	/// \param entity Entity who shoot.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity) const;
};

/// AABB query callback that indicate if a corpse is found into the AABB.
class CorpseQueryCallback : public b2QueryCallback
{
	public:
		/// Handle the world querying.
		/// \param fixture A fixture that overlap the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		entityx::Entity foundEntity;///< If no corpse is found, then this entity won't be valid.
};

#endif//CORPSESEARCHER_H

