#ifndef MOVER_H
#define MOVER_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}
enum class Direction;

/// Structure that move an entity to a given side.
struct Mover : public Action
{
	/// Default constructor.
	/// \param _direction
	/// \param _start True if the movement starts, false if it stops.
	Mover(Direction _direction, bool _start = true);

	/// Default destructor
	virtual ~Mover();

	/// Overload of the () operator.
	/// \param entity Entity who move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;

	Direction direction;///< Indicates the direction of the movement.
	bool start;         ///< True if the movement starts, false if it stops.
};

#endif//MOVER_H

