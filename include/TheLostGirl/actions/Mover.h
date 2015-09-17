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
	/// Constructor.
	/// \param _direction
	/// \param _start True if the movement starts, false if it stops.
	Mover(Direction _direction, bool _start = true);

	/// Destructor.
	virtual ~Mover();

	/// Overload of the () operator.
	/// \param entity Entity who move.
	/// \param dt Elapsed time in the last game frame.
	virtual void operator()(entityx::Entity entity, double dt) const;


	void flip(entityx::Entity entity) const;

	float32 getMid(b2Body* body) const;

	void flipFixtures(b2Body* body, float32 mid) const;

	inline void flipBody(b2Body* body, float32 mid) const;

	inline void flipPoint(b2Vec2& vec, float32 mid) const;

	Direction direction;///< Indicates the direction of the movement.
	bool start;         ///< True if the movement starts, false if it stops.
};

#endif//MOVER_H

