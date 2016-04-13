#ifndef COMMANDTESTS_HPP
#define COMMANDTESTS_HPP

#include <entityx/entityx.h>
#include <TheLostGirl/Command.hpp>
#include <tests/TestsFixture.hpp>

class CommandTestsFixture : public TestsFixture
{
	public:
		/// Constructor.
		CommandTestsFixture();

		/// Destructor.
		~CommandTestsFixture();

		entityx::EventManager eventManager;
		entityx::EntityManager entityManager;
		entityx::Entity entity;
		FlagSet<Category> category;
		entityx::Entity otherEntity;
		FlagSet<Category>otherCategory;
		Command targetSpecific;
		Command targetNotSpecific;
};

void action(entityx::Entity entity);

void otherAction(entityx::Entity entity);

#endif//COMMANDTESTS_HPP
