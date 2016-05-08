#ifndef ARROWPICKER_HPP
#define ARROWPICKER_HPP

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/Context.hpp>

/// Structure that picks a sticked arrow nearby the entity.
class ArrowPicker : public Action
{
	public:
		/// Constructor.
		ArrowPicker() = default;

		/// Destructor.
		virtual ~ArrowPicker() = default;

		/// Overload of the () operator.
		/// \param entity Entity that picks up an arrow.
		virtual void operator()(entityx::Entity entity) const override;

	private:
		/// Context type of this class.
		typedef ContextAccessor<ContextElement::Parameters> Context;
};

/// AABB query callback that indicates if a sticked arrow is found into the AABB.
class StickedArrowQueryCallback : public b2QueryCallback
{
	public:
		/// Handles the world querying.
		/// \param fixture A fixture that overlaps the AABB.
		/// \return True if the querying should continue, false otherwise.
		virtual bool ReportFixture(b2Fixture* fixture);

		entityx::Entity foundEntity;///< If no arrow is found, then this entity won't be valid.
};

#endif//ARROWPICKER_HPP
