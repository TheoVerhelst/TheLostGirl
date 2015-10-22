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
	/// Constructor.
	/// \param context Current context of the application.
	Death(StateStack::Context context);

	/// Destructor.
	virtual ~Death() = default;

	/// Overload of the () operator.
	/// \param entity Entity who dead.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;

	private:
		StateStack::Context m_context;///< Current context of the application.
};

#endif//DEATH_H

