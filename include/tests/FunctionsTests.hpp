#ifndef FUNCTIONSTESTS_HPP
#define FUNCTIONSTESTS_HPP

#include <Box2D/Box2D.h>
#include <tests/TestsFixture.hpp>

class FunctionsTestsFixture : public TestsFixture
{
	public:
		constexpr static float m_tolerance{0.00001f};
};

class GetJointDefFixture : public TestsFixture
{
	public:
		/// Constructor.
		GetJointDefFixture();

		/// Destructor.
		~GetJointDefFixture();

		void testJointDefinition(b2JointDef* jointDefinition);

		constexpr static float m_tolerance{0.00001f};
		b2World world;
		b2BodyDef bodyDefinition;
		b2Body* bodyA;
		b2Body* bodyB;
		bool collideConnected;
		void* userData;
};

constexpr std::size_t operator "" _z(unsigned long long n);

constexpr void* operator "" _vptr(unsigned long long n);

#endif//FUNCTIONSTESTS_HPP
