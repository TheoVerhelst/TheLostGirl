#include <boost/test/unit_test.hpp>
#include <TheLostGirl/components.hpp>
#include <tests/HashEntityTests.hpp>

HashEntityTestsFixture::HashEntityTestsFixture():
	entities(events),
	entity1(entities.create()),
	entity2(entities.create()),
	entity3(entity1)
{
}

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
