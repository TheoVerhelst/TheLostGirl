#ifndef ARROWSHOOTER_HPP
#define ARROWSHOOTER_HPP

#include <TheLostGirl/Action.hpp>

namespace entityx
{
	class Entity;
}

/// Structure that shoot the arrow notched by the entity.
class ArrowShooter : public Action
{
	public:
		/// Constructor.
		ArrowShooter() = default;

		/// Destructor.
		virtual ~ArrowShooter() = default;

		/// Overload of the () operator.
		/// \param entity Entity that shoots.
		virtual void operator()(entityx::Entity entity) const override;
};

#endif//ARROWSHOOTER_HPP
