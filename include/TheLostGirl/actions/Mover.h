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

	float32 flipFixtures(b2Body* body) const;

	inline void flipPoint(b2Vec2& vec, float32 mid) const;

	template <typename JointDef, typename Joint>
	Joint* flipJoint(Joint* joint, float32 mid, entityx::Entity entity) const
	{
		b2Body* body{joint->GetBodyB()};
		b2World* world{body->GetWorld()};
		//Flip locally all fixtures of the body, and get the flipping axis
		const float32 centerX{flipFixtures(body)};
		//Get the definitions of the fixtures, the body and the joint
		std::list<b2FixtureDef> fixtureDefs;
		for(b2Fixture* fixture{body->GetFixtureList()}; fixture; fixture = fixture->GetNext())
			fixtureDefs.push_back(getFixtureDef(fixture));
		b2BodyDef bodyDef{getBodyDef(body)};
		JointDef* jointDef{static_cast<JointDef*>(getJointDef(joint))};

		//Set the position in the body definition (main goal of this operation)
		const float32 intialX{bodyDef.position.x};
		flipPoint(bodyDef.position, mid);
		bodyDef.position.x -= (centerX - intialX) * 2.f;

		//Destroy the body and recreate it, with its fixtures
		body = world->CreateBody(&bodyDef);
		for(; not fixtureDefs.empty(); fixtureDefs.pop_back())
			body->CreateFixture(&fixtureDefs.back());
		//The old body is destroyed after because the new fixtures definitions where using
		//pointers to shapes held by the old fixtures, which are deleted when the body is destroyed
		world->DestroyBody(jointDef->bodyB);

		//Recreate the joint
		std::cout << "body = " << body;
		std::cout << "\tA = " << jointDef->bodyA;
		std::cout << "\tB = " << jointDef->bodyB << std::endl;
		jointDef->bodyB = body;
		jointDef->localAnchorB = body->GetLocalPoint(jointDef->bodyA->GetWorldPoint(jointDef->localAnchorA));
		jointDef->referenceAngle = body->GetAngle() - jointDef->bodyA->GetAngle();
		entity.component<BodyComponent>()->body = body;
		return static_cast<Joint*>(world->CreateJoint(jointDef));
	}

	Direction direction;///< Indicates the direction of the movement.
	bool start;         ///< True if the movement starts, false if it stops.
};

#endif//MOVER_H

