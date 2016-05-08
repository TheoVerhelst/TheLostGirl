#ifndef JUMPER_HPP
#define JUMPER_HPP

#include <TheLostGirl/Action.hpp>

namespace entityx
{
	class Entity;
}

/// Structure that jump an entity.
class Jumper : public Action
{
	public:
		/// Constructor.
		Jumper() = default;

		/// Destructor.
		virtual ~Jumper() = default;

		/// Overload of the () operator.
		/// \param entity Entity that jumps.
		virtual void operator()(entityx::Entity entity) const override;
};

#endif//JUMPER_HPP
