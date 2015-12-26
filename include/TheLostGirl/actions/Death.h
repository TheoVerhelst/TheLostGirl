#ifndef DEATH_H
#define DEATH_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Structure that process the death of an entity
class Death : public Action
{
	public:
		/// Destructor.
		virtual ~Death() = default;

		/// Overload of the () operator.
		/// \param entity Entity that deads.
		virtual void operator()(entityx::Entity entity) const override;
};

#endif//DEATH_H

