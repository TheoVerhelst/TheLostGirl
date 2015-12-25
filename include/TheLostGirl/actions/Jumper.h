#ifndef JUMPER_H
#define JUMPER_H

#include <TheLostGirl/Action.h>

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
		/// \param entity Entity who jump.
		virtual void operator()(entityx::Entity entity) const override;
};

#endif//JUMPER_H

