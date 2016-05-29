#ifndef ARROWPICKER_HPP
#define ARROWPICKER_HPP

#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/Action.hpp>
#include <TheLostGirl/Context.hpp>
#include <TheLostGirl/Box2DHelper.hpp>

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
		
		/// Checks whether the entity is a sticked arrow or is not moving, it is
		/// used to find the arrow to pick up.
		/// \param entity The entity to check.
		/// \return true if the entity is a sticked arrow or is not moving,
		/// false otherwhise.
		static bool isPickableArrow(entityx::Entity entity);
		
		static constexpr float m_speedThreshold{0.001f};///< The maximum speed at wich an arrow is considered to be not moving.
};


#endif//ARROWPICKER_HPP
