#include <boost/test/unit_test.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/HashEntity.hpp>

struct HashEntityTestsFixture
{
	HashEntity hasher;
	entityx::EventManager events;
	entityx::EntityManager entities;
	entityx::Entity entity1, entity2, entity3;
	HashEntityTestsFixture():
		entities(events),
		entity1(entities.create()),
		entity2(entities.create()),
		entity3(entity1)
	{
	}

	~HashEntityTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(HashEntityTests, HashEntityTestsFixture)

BOOST_AUTO_TEST_CASE(hash)
{
	BOOST_CHECK_NE(hasher(entity1), hasher(entity2));
	BOOST_CHECK_EQUAL(hasher(entity1), hasher(entity3));
	entity1.assign<BodyComponent>();
	BOOST_CHECK_NE(hasher(entity1), hasher(entity2));
	BOOST_CHECK_EQUAL(hasher(entity1), hasher(entity3));
	entity2.assign<BodyComponent>();
	BOOST_CHECK_NE(hasher(entity1), hasher(entity2));
	BOOST_CHECK_EQUAL(hasher(entity1), hasher(entity3));
	entity1.assign<ItemComponent>();
	BOOST_CHECK_NE(hasher(entity1), hasher(entity2));
	BOOST_CHECK_EQUAL(hasher(entity1), hasher(entity3));
}

BOOST_AUTO_TEST_SUITE_END()
