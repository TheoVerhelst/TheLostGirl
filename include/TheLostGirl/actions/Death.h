#ifndef DEATH_H
#define DEATH_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}

/// Structure that process the death of an entity
struct Death : public Action
{
	/// Destructor.
	virtual ~Death() = default;

	/// Overload of the () operator.
	/// \param entity Entity who dead.
	virtual void operator()(entityx::Entity entity) const;
};

#endif//DEATH_H

