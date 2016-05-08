#ifndef CORPSESEARCHER_HPP
#define CORPSESEARCHER_HPP

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/StateStack.hpp>
#include <TheLostGirl/Context.hpp>

/// Structure that searches on a corpse around the entity.
class CorpseSearcher : public Action
{
	public:
		/// Destructor.
		virtual ~CorpseSearcher() = default;

		/// Overload of the () operator.
		/// \param entity Entity that search in corpses.
		virtual void operator()(entityx::Entity entity) const override;

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::StateStack> Context;
};

/// AABB query callback that indicates if a corpse is found into the AABB.
class CorpseQueryCallback : public b2QueryCallback
{
	public:
		/// Handles the world querying.
		/// \param fixture A fixture that overlaps the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		entityx::Entity foundEntity;///< If no corpse is found, then this entity won't be valid.
};

#endif//CORPSESEARCHER_HPP
