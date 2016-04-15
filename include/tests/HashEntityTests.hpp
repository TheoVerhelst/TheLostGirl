#ifndef HASHENTITYTESTS_HPP
#define HASHENTITYTESTS_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/HashEntity.hpp>
#include <tests/TestsFixture.hpp>

class HashEntityTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		HashEntityTestsFixture();

		HashEntity hasher;
		entityx::EventManager events;
		entityx::EntityManager entities;
		entityx::Entity entity1, entity2, entity3;
};

#endif//HASHENTITYTESTS_HPP
