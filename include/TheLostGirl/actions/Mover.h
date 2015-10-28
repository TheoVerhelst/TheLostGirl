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
		/// \param _direction The direction of the movement.
		/// \param _start True if the movement starts, false if it stops.
		Mover( StateStack::Context context, Direction _direction, bool _start = true);

		/// Destructor.
		virtual ~Mover() = default;

		/// Overload of the () operator.
		/// \param entity Entity who move.
		virtual void operator()(entityx::Entity entity) const;

		Direction direction;///< Indicates the direction of the movement.
		bool start;         ///< True if the movement starts, false if it stops.

	private:
		/// Flip an entity.
		/// \param entity The entity to flip.
		void flip(entityx::Entity entity) const;

		/// Computes the half width of a body.
		/// \param body The body used to compute.
		/// \return The half width of a body.
		inline float32 getMid(b2Body* body) const;

		/// Flip all fixtures of the \a body.
		/// \param body The body to flip.
		/// \param mid Thehalf width of the \a body.
		void flipFixtures(b2Body* body, float32 mid) const;

		/// Flip the \a body.
		/// \param body The body to flip.
		/// \param mid Thehalf width of the \a body.
		inline void flipBody(b2Body* body, float32 mid) const;

		/// Flip a point around x = mid.
		/// \param vec The point to flip.
		/// \param mid Thehalf width of the body.
		inline void flipPoint(b2Vec2& vec, float32 mid) const;

		StateStack::Context m_context;///< Current context of the application.
};

#endif//MOVER_H

