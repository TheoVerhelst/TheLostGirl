#ifndef MOVER_H
#define MOVER_H

#include <Box2D/Dynamics/Joints/b2PrismaticJoint.h>
#include <Box2D/Dynamics/Joints/b2WeldJoint.h>
#include <Box2D/Dynamics/Joints/b2RevoluteJoint.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/Joints/b2Joint.h>
#include <TheLostGirl/Action.h>
#include <TheLostGirl/functions.h>

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
		/// \param context Current context of the application.
		/// \param _direction
		/// \param _start True if the movement starts, false if it stops.
		Mover( StateStack::Context context, Direction _direction, bool _start = true);

		/// Destructor.
		virtual ~Mover() = default;

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

		StateStack::Context m_context;///< Current context of the application.
};

#endif//MOVER_H

