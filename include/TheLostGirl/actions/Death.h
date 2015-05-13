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
	/// Default constructor.
	Death(StateStack::Context context);

	/// Default destructor
	virtual ~Death();

	/// Overload of the () operator.
	/// \param entity Entity who dead.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;

	private:
		StateStack::Context m_context;
};

#endif//DEATH_H

