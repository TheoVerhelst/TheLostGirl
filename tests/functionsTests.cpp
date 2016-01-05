#include <boost/test/unit_test.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>

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
	catComponent->category.set(Category::Scene);
	BOOST_CHECK(not isPlayer(entity));
	catComponent->category.set(Category::Player);
	BOOST_CHECK(isPlayer(entity));
}

BOOST_AUTO_TEST_CASE(getBodyDefTests)
{
	b2World world({0.f, 1.f});

	b2BodyDef def;
	def.active = true;
	def.allowSleep = true;
	def.awake = true;
	def.bullet = false;
	def.fixedRotation = false;
	def.angle = 2.f;
	def.angularDamping = 1.f;
	def.angularVelocity = 2.3f;
	def.gravityScale = 1.3f;
	def.linearDamping = 5.3f;
	def.linearVelocity = {2.5f, 1.1f};
	def.position = {55.f, 100.4f};
	def.type = b2_dynamicBody;
	def.userData = 0b0110101_vptr;

	b2BodyDef newDef = getBodyDef(world.CreateBody(&def));
	BOOST_CHECK_EQUAL(def.active, newDef.active);
	BOOST_CHECK_EQUAL(def.allowSleep, newDef.allowSleep);
	BOOST_CHECK_EQUAL(def.awake, newDef.awake);
	BOOST_CHECK_EQUAL(def.bullet, newDef.bullet);
	BOOST_CHECK_EQUAL(def.fixedRotation, newDef.fixedRotation);
	BOOST_CHECK_EQUAL(def.userData, newDef.userData);
	BOOST_CHECK_CLOSE(def.angle, newDef.angle, 0.0001);
	BOOST_CHECK_CLOSE(def.angularDamping, newDef.angularDamping, 0.0001);
	BOOST_CHECK_CLOSE(def.angularVelocity, newDef.angularVelocity, 0.0001);
	BOOST_CHECK_CLOSE(def.gravityScale, newDef.gravityScale, 0.0001);
	BOOST_CHECK_CLOSE(def.linearDamping, newDef.linearDamping, 0.0001);
	BOOST_CHECK_CLOSE(def.linearVelocity.x, newDef.linearVelocity.x, 0.0001);
	BOOST_CHECK_CLOSE(def.linearVelocity.y, newDef.linearVelocity.y, 0.0001);
	BOOST_CHECK_CLOSE(def.position.x, newDef.position.x, 0.0001);
	BOOST_CHECK_CLOSE(def.position.y, newDef.position.y, 0.0001);
}

BOOST_AUTO_TEST_CASE(getFixtureDefTests)
{
	b2World world({0.f, 1.f});
	b2BodyDef bodyDef;
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(2.f, 5.f);

	b2Filter filter;
	filter.categoryBits = 0b0101u;
	filter.groupIndex = 3;
	filter.maskBits = 0b10001u;

	b2FixtureDef def;
	def.isSensor = false;
	def.density = 0.312f;
	def.friction = 0.949f;
	def.restitution = 0.123f;
	def.filter = filter;
	def.userData = 0b0110101_vptr;
	def.shape = &shape;

	b2FixtureDef newDef = getFixtureDef(body->CreateFixture(&def));
	BOOST_CHECK_EQUAL(def.isSensor, newDef.isSensor);
	BOOST_CHECK_EQUAL(def.userData, newDef.userData);
	BOOST_CHECK_EQUAL(def.filter.categoryBits, newDef.filter.categoryBits);
	BOOST_CHECK_EQUAL(def.filter.groupIndex, newDef.filter.groupIndex);
	BOOST_CHECK_EQUAL(def.filter.maskBits, newDef.filter.maskBits);
	BOOST_CHECK_EQUAL(def.shape->m_type, newDef.shape->m_type);
	BOOST_CHECK_CLOSE(def.shape->m_radius, newDef.shape->m_radius, 0.0001);
	BOOST_CHECK_CLOSE(def.density, newDef.density, 0.0001);
	BOOST_CHECK_CLOSE(def.friction, newDef.friction, 0.0001);
	BOOST_CHECK_CLOSE(def.restitution, newDef.restitution, 0.0001);
}

BOOST_AUTO_TEST_CASE(getJointDefTests)
{
	//TODO Implements this test (alot of code)
}

BOOST_AUTO_TEST_CASE(operatorNotTests)
{
	BOOST_CHECK((not Direction::Left) == Direction::Right);
	BOOST_CHECK((not Direction::Right) == Direction::Left);
	BOOST_CHECK((not Direction::Up) == Direction::Down);
	BOOST_CHECK((not Direction::Down) == Direction::Up);
	BOOST_CHECK((not Direction::None) == Direction::None);
}

BOOST_AUTO_TEST_SUITE_END()
