#include <boost/test/unit_test.hpp>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/functions.h>

struct functionsTestsFixture
{
	functionsTestsFixture()
	{
	}

	~functionsTestsFixture()
	{
	}
};

BOOST_FIXTURE_TEST_SUITE(functionsTests, functionsTestsFixture)

BOOST_AUTO_TEST_CASE(capTests)
{
	BOOST_CHECK_EQUAL(cap(5, -10, 10), 5);
	BOOST_CHECK_EQUAL(cap(0, -15, 5), 0);
	BOOST_CHECK_EQUAL(cap(16, 10, 130), 16);
	BOOST_CHECK_EQUAL(cap(15, -5, 10), 10);
	BOOST_CHECK_EQUAL(cap(-15, -15, 17), -15);
	BOOST_CHECK_EQUAL(cap(-44, -21, 10), -21);
}

BOOST_AUTO_TEST_CASE(fadingColorTests)
{
	BOOST_CHECK_EQUAL(fadingColor(sf::seconds(0.f), sf::seconds(1.f), true).a, 0);
	BOOST_CHECK(fadingColor(sf::seconds(3.f), sf::seconds(3.f), true) == sf::Color::White);
	BOOST_CHECK(fadingColor(sf::seconds(0.f), sf::seconds(1.f), false) == sf::Color::White);
	BOOST_CHECK_EQUAL(fadingColor(sf::seconds(3.f), sf::seconds(3.f), false).a, 0);
}

BOOST_AUTO_TEST_CASE(hasWhiteSpaceTests)
{
	BOOST_CHECK(hasWhiteSpace("string whith spaces"));
	BOOST_CHECK(hasWhiteSpace("string\nwhith\nnewlines"));
	BOOST_CHECK(hasWhiteSpace("string\twhith\ntabs"));
	BOOST_CHECK(not hasWhiteSpace("stringwhithoutspaces"));
}

BOOST_AUTO_TEST_CASE(b2tosfTests)
{
	const b2Vec2 b2vec(3.f, 2.f);
	const sf::Vector2f sfvec(b2tosf(b2vec));
	BOOST_CHECK_CLOSE(b2vec.x, sfvec.x, 0.001);
	BOOST_CHECK_CLOSE(b2vec.y, sfvec.y, 0.001);
}

BOOST_AUTO_TEST_CASE(sftob2Tests)
{
	const sf::Vector2f sfvec(3.f, 2.f);
	const b2Vec2 b2vec(sftob2(sfvec));
	BOOST_CHECK_CLOSE(b2vec.x, sfvec.x, 0.001);
	BOOST_CHECK_CLOSE(b2vec.y, sfvec.y, 0.001);
}

BOOST_AUTO_TEST_CASE(b2ColorToSfTests)
{
	BOOST_CHECK(b2ColorToSf(b2Color(1.f, 0.f, 0.f, 1.f)) == sf::Color::Red);
	BOOST_CHECK(b2ColorToSf(b2Color(0.f, 1.f, 0.f, 1.f)) == sf::Color::Green);
	BOOST_CHECK(b2ColorToSf(b2Color(0.f, 0.f, 1.f, 1.f)) == sf::Color::Blue);
	BOOST_CHECK_EQUAL(b2ColorToSf(b2Color(0.f, 0.f, 0.f, 0.f)).a, sf::Color::Transparent.a);
}

constexpr std::size_t operator "" _z(unsigned long long n)
{
	return static_cast<std::size_t>(n);
}

constexpr void* operator "" _vptr(unsigned long long n)
{
	return reinterpret_cast<void*>(n);
}

BOOST_AUTO_TEST_CASE(addTests)
{
	BOOST_CHECK_EQUAL(add(0b000_vptr, 0b100_z), 0b100_vptr);
	BOOST_CHECK_EQUAL(add(0b010_vptr, 0b100_z), 0b110_vptr);
	BOOST_CHECK_EQUAL(add(0b100_vptr, 0b100_z), 0b100_vptr);
	BOOST_CHECK_EQUAL(add(0b111_vptr, 0b000_z), 0b111_vptr);
}

BOOST_AUTO_TEST_CASE(isMemberTests)
{
	std::map<int, int> test_map;
	test_map.emplace(12, 3);
	test_map.emplace(3, 5);
	test_map.emplace(6, 3);
	test_map.emplace(1, 7);
	BOOST_CHECK(isMember(test_map, 3));
	BOOST_CHECK(isMember(test_map, 5));
	BOOST_CHECK(not isMember(test_map, 1));
}

BOOST_AUTO_TEST_CASE(getKeyTests)
{
	std::map<int, int> test_map;
	test_map.emplace(12, 3);
	test_map.emplace(3, 5);
	test_map.emplace(6, 3);
	test_map.emplace(1, 7);
	BOOST_CHECK_EQUAL(getKey(test_map, 5), 3);
	BOOST_CHECK_EQUAL(getKey(test_map, 7), 1);
	BOOST_CHECK(getKey(test_map, 3) == 12 or getKey(test_map, 3) == 6);
}

BOOST_AUTO_TEST_CASE(isPlayerTests)
{
	entityx::EventManager events;
	entityx::EntityManager entities(events);
	entityx::Entity entity;
	BOOST_CHECK(not isPlayer(entity));
	entity = entities.create();
	CategoryComponent::Handle catComponent = entity.assign<CategoryComponent>();
	catComponent->category |= Category::Scene;
	BOOST_CHECK(not isPlayer(entity));
	catComponent->category |= Category::Player;
	BOOST_CHECK(isPlayer(entity));
}

BOOST_AUTO_TEST_CASE(getBodyDefTests)
{
}

BOOST_AUTO_TEST_CASE(getFixtureDefTests)
{
}

BOOST_AUTO_TEST_CASE(getJointDefTests)
{
}

BOOST_AUTO_TEST_CASE(operatorNotTests)
{
}

BOOST_AUTO_TEST_SUITE_END()
