#ifndef ANIMATIONSMANAGERTESTS_HPP
#define ANIMATIONSMANAGERTESTS_HPP

#include <TheLostGirl/AnimationsManager.hpp>
#include <tests/TestsFixture.hpp>

class MockAnimation
{
	public:
		void animate(float);
};

class AnimationsManagerTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		AnimationsManagerTestsFixture();

		/// Destructor.
		~AnimationsManagerTestsFixture();

		AnimationsManager<MockAnimation> manager;
};

#endif//ANIMATIONSMANAGERTESTS_HPP
