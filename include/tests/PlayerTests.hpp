#ifndef PLAYERTESTS_HPP
#define PLAYERTESTS_HPP

#include <entityx/entityx.h>
#include <tests/TestsFixture.hpp>

class PlayerTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		PlayerTestsFixture();

		/// Destructor.
		~PlayerTestsFixture();

		entityx::EventManager events;
		entityx::EntityManager entities;
		entityx::SystemManager systems;
};

#endif//PLAYERTESTS_HPP
