#ifndef MOVER_H
#define MOVER_H

#include <TheLostGirl/Action.h>

namespace entityx
{
    class Entity;
}
enum class Direction;
class b2WeldJointDef;
class b2WeldJoint;

/// Structure that move an entity to a given side.
struct Mover : public Action
{
	public:
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

		Direction direction;///< Indicates the direction of the movement.
		bool start;         ///< True if the movement starts, false if it stops.

	private:
		void flip(entityx::Entity entity) const;

		inline float32 getMid(b2Body* body) const;

		void flipFixtures(b2Body* body, float32 mid) const;

		inline void flipBody(b2Body* body, float32 mid) const;

		inline void flipPoint(b2Vec2& vec, float32 mid) const;

		b2WeldJointDef getWeldJointDef(b2WeldJoint* joint) const;
};

#endif//MOVER_H

