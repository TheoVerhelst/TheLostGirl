#include <sstream>
#include <boost/test/unit_test.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <TheLostGirl/components.hpp>
#include <TheLostGirl/functions.hpp>
#include <tests/FunctionsTests.hpp>

GetJointDefFixture::GetJointDefFixture():
	world{{2.f, 3.f}},
	bodyDefinition{},
	bodyA{world.CreateBody(&bodyDefinition)},
	bodyB{world.CreateBody(&bodyDefinition)},
	collideConnected{false},
	userData{0b0110101_vptr}
{
}

GetJointDefFixture::~GetJointDefFixture()
{
	world.DestroyBody(bodyA);
	world.DestroyBody(bodyB);
}

void GetJointDefFixture::testJointDefinition(b2JointDef* jointDefinition)
{
	b2Joint* joint{world.CreateJoint(jointDefinition)};
	b2JointDef* result{getJointDef(joint)};

	switch(jointDefinition->type)
	{
		case e_revoluteJoint:
		{
			BOOST_REQUIRE(jointDefinition->type == e_revoluteJoint);
			auto castedJointDef = static_cast<b2RevoluteJointDef*>(jointDefinition);
			auto castedResult = static_cast<b2RevoluteJointDef*>(result);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorA - castedResult->localAnchorA).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorB - castedResult->localAnchorB).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL(castedJointDef->referenceAngle, castedResult->referenceAngle);
			BOOST_CHECK_EQUAL(castedJointDef->enableLimit, castedResult->enableLimit);
			BOOST_CHECK_EQUAL(castedJointDef->lowerAngle, castedResult->lowerAngle);
			BOOST_CHECK_EQUAL(castedJointDef->upperAngle, castedResult->upperAngle);
			BOOST_CHECK_EQUAL(castedJointDef->enableMotor, castedResult->enableMotor);
			BOOST_CHECK_EQUAL(castedJointDef->motorSpeed, castedResult->motorSpeed);
			BOOST_CHECK_EQUAL(castedJointDef->maxMotorTorque, castedResult->maxMotorTorque);
			break;
		}

		case e_prismaticJoint:
		{
			BOOST_REQUIRE(jointDefinition->type == e_prismaticJoint);
			auto castedJointDef = static_cast<b2PrismaticJointDef*>(jointDefinition);
			auto castedResult = static_cast<b2PrismaticJointDef*>(result);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorA - castedResult->localAnchorA).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorB - castedResult->localAnchorB).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL((castedJointDef->localAxisA - castedResult->localAxisA).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL(castedJointDef->referenceAngle, castedResult->referenceAngle);
			BOOST_CHECK_EQUAL(castedJointDef->lowerTranslation, castedResult->lowerTranslation);
			BOOST_CHECK_EQUAL(castedJointDef->upperTranslation, castedResult->upperTranslation);
			BOOST_CHECK_EQUAL(castedJointDef->enableLimit, castedResult->enableLimit);
			BOOST_CHECK_EQUAL(castedJointDef->maxMotorForce, castedResult->maxMotorForce);
			BOOST_CHECK_EQUAL(castedJointDef->motorSpeed, castedResult->motorSpeed);
			BOOST_CHECK_EQUAL(castedJointDef->enableMotor, castedResult->enableMotor);
			break;
		}

		case e_weldJoint:
		{
			BOOST_REQUIRE(jointDefinition->type == e_weldJoint);
			auto castedJointDef = static_cast<b2WeldJointDef*>(jointDefinition);
			auto castedResult = static_cast<b2WeldJointDef*>(result);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorA - castedResult->localAnchorA).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL((castedJointDef->localAnchorB - castedResult->localAnchorB).LengthSquared(), 0.f);
			BOOST_CHECK_EQUAL(castedJointDef->referenceAngle, castedResult->referenceAngle);
			BOOST_CHECK_EQUAL(castedJointDef->frequencyHz, castedResult->frequencyHz);
			BOOST_CHECK_EQUAL(castedJointDef->dampingRatio, castedResult->dampingRatio);
			break;
		}

		default:
			BOOST_CHECK(result == nullptr);
			return;
	}

	BOOST_CHECK_EQUAL(jointDefinition->type, result->type);
	BOOST_CHECK_EQUAL(jointDefinition->bodyA, result->bodyA);
	BOOST_CHECK_EQUAL(jointDefinition->bodyB, result->bodyB);
	BOOST_CHECK_EQUAL(jointDefinition->collideConnected, result->collideConnected);
	BOOST_CHECK_EQUAL(jointDefinition->userData, result->userData);

	world.DestroyJoint(joint);
	delete result;
}

constexpr std::size_t operator "" _z(unsigned long long n)
{
	return static_cast<std::size_t>(n);
}

constexpr void* operator "" _vptr(unsigned long long n)
{
	return reinterpret_cast<void*>(n);
}

BOOST_FIXTURE_TEST_SUITE(FunctionsTests, FunctionsTestsFixture)

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

BOOST_AUTO_TEST_CASE(b2tosfTests, *boost::unit_test::tolerance(FunctionsTestsFixture::m_tolerance))
{
	const b2Vec2 b2vec(3.f, 2.f);
	const sf::Vector2f sfvec(b2tosf(b2vec));
	BOOST_CHECK_EQUAL(b2vec.x, sfvec.x);
	BOOST_CHECK_EQUAL(b2vec.y, sfvec.y);
}

BOOST_AUTO_TEST_CASE(sftob2Tests, *boost::unit_test::tolerance(FunctionsTestsFixture::m_tolerance))
{
	const sf::Vector2f sfvec(3.f, 2.f);
	const b2Vec2 b2vec(sftob2(sfvec));
	BOOST_CHECK_EQUAL(b2vec.x, sfvec.x);
	BOOST_CHECK_EQUAL(b2vec.y, sfvec.y);
}

BOOST_AUTO_TEST_CASE(b2ColorToSfTests)
{
	BOOST_CHECK(b2ColorToSf(b2Color(1.f, 0.f, 0.f, 1.f)) == sf::Color::Red);
	BOOST_CHECK(b2ColorToSf(b2Color(0.f, 1.f, 0.f, 1.f)) == sf::Color::Green);
	BOOST_CHECK(b2ColorToSf(b2Color(0.f, 0.f, 1.f, 1.f)) == sf::Color::Blue);
	BOOST_CHECK_EQUAL(b2ColorToSf(b2Color(0.f, 0.f, 0.f, 0.f)).a, sf::Color::Transparent.a);
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
	entityx::Entity entity = entities.create();
	BOOST_CHECK(not isPlayer(entity));
	entity = entities.create();
	CategoryComponent::Handle catComponent = entity.assign<CategoryComponent>();
	catComponent->category.set(Category::Scene);
	BOOST_CHECK(not isPlayer(entity));
	catComponent->category.set(Category::Player);
	BOOST_CHECK(isPlayer(entity));
}

BOOST_AUTO_TEST_CASE(getBodyDefTests, *boost::unit_test::tolerance(FunctionsTestsFixture::m_tolerance))
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
	BOOST_CHECK_EQUAL(def.angle, newDef.angle);
	BOOST_CHECK_EQUAL(def.angularDamping, newDef.angularDamping);
	BOOST_CHECK_EQUAL(def.angularVelocity, newDef.angularVelocity);
	BOOST_CHECK_EQUAL(def.gravityScale, newDef.gravityScale);
	BOOST_CHECK_EQUAL(def.linearDamping, newDef.linearDamping);
	BOOST_CHECK_EQUAL(def.linearVelocity.x, newDef.linearVelocity.x);
	BOOST_CHECK_EQUAL(def.linearVelocity.y, newDef.linearVelocity.y);
	BOOST_CHECK_EQUAL(def.position.x, newDef.position.x);
	BOOST_CHECK_EQUAL(def.position.y, newDef.position.y);
}

BOOST_AUTO_TEST_CASE(getFixtureDefTests, *boost::unit_test::tolerance(FunctionsTestsFixture::m_tolerance))
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
	BOOST_CHECK_EQUAL(def.shape->m_radius, newDef.shape->m_radius);
	BOOST_CHECK_EQUAL(def.density, newDef.density);
	BOOST_CHECK_EQUAL(def.friction, newDef.friction);
	BOOST_CHECK_EQUAL(def.restitution, newDef.restitution);
}

BOOST_AUTO_TEST_CASE(operatorNotDirectionTests)
{
	BOOST_CHECK((not Direction::Left) == Direction::Right);
	BOOST_CHECK((not Direction::Right) == Direction::Left);
	BOOST_CHECK((not Direction::Up) == Direction::Down);
	BOOST_CHECK((not Direction::Down) == Direction::Up);
	BOOST_CHECK((not Direction::None) == Direction::None);
}

BOOST_AUTO_TEST_CASE(streamOperatorDirectionTests)
{
	std::stringstream stream;
	std::vector<std::pair<Direction, std::string>> expectedMessages{
			{Direction::Left, "Direction::Left"},
			{Direction::Right, "Direction::Right"},
			{Direction::Up, "Direction::Up"},
			{Direction::Down, "Direction::Down"},
			{Direction::None, "Direction::None"}};
	for(const auto& expectedMessage : expectedMessages)
	{
		stream.str("");
		stream << expectedMessage.first;
		BOOST_CHECK(stream.str() == expectedMessage.second);
	}
}

BOOST_AUTO_TEST_CASE(printErrorTests)
{
	const std::string filename{"test.cpp"}, expression{"erroneous_line();"};
	const unsigned int lineNumber{42U};
	logStream(std::cerr);
	BOOST_CHECK(not printError(expression, filename, lineNumber));
	std::string errorMessage{getStreamLog()};
	// Check that the error message contains required information
	BOOST_CHECK(errorMessage.find(expression) != std::string::npos);
	BOOST_CHECK(errorMessage.find(filename) != std::string::npos);
	BOOST_CHECK(errorMessage.find(std::to_string(lineNumber)) != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(getJointDefTests, GetJointDefFixture)

BOOST_AUTO_TEST_CASE(RevoluteJoint, *boost::unit_test::tolerance(GetJointDefFixture::m_tolerance))
{
	b2RevoluteJointDef jointDefinition;
	jointDefinition.bodyA = bodyA;
	jointDefinition.bodyB = bodyB;
	jointDefinition.collideConnected = collideConnected;
	jointDefinition.userData = userData;
	jointDefinition.localAnchorA = {3.f, 4.f};
	jointDefinition.localAnchorB = {5.f, 6.f};
	jointDefinition.referenceAngle = 3.f;
	jointDefinition.enableLimit = true;
	jointDefinition.lowerAngle = 4.f;
	jointDefinition.upperAngle = 5.f;
	jointDefinition.enableMotor = false;
	jointDefinition.motorSpeed = 6.f;
	jointDefinition.maxMotorTorque = 7.f;
	testJointDefinition(&jointDefinition);
}

BOOST_AUTO_TEST_CASE(PrismaticJoint, *boost::unit_test::tolerance(GetJointDefFixture::m_tolerance))
{
	b2PrismaticJointDef jointDefinition;
	jointDefinition.bodyA = bodyA;
	jointDefinition.bodyB = bodyB;
	jointDefinition.collideConnected = collideConnected;
	jointDefinition.userData = userData;
	jointDefinition.localAnchorA = {3.f, 4.f};
	jointDefinition.localAnchorB = {5.f, 6.f};
	jointDefinition.localAxisA = {7.f, 8.f};
	jointDefinition.localAxisA.Normalize();
	jointDefinition.referenceAngle = 3.f;
	jointDefinition.enableLimit = true;
	jointDefinition.lowerTranslation = 4.f;
	jointDefinition.upperTranslation = 5.f;
	jointDefinition.enableMotor = false;
	jointDefinition.motorSpeed = 6.f;
	jointDefinition.maxMotorForce = 7.f;
	testJointDefinition(&jointDefinition);
}

BOOST_AUTO_TEST_CASE(WeldJoint, *boost::unit_test::tolerance(GetJointDefFixture::m_tolerance))
{
	b2WeldJointDef jointDefinition;
	jointDefinition.bodyA = bodyA;
	jointDefinition.bodyB = bodyB;
	jointDefinition.collideConnected = collideConnected;
	jointDefinition.userData = userData;
	jointDefinition.localAnchorA = {3.f, 4.f};
	jointDefinition.localAnchorB = {5.f, 6.f};
	jointDefinition.referenceAngle = 3.f;
	jointDefinition.frequencyHz = 4.f;
	jointDefinition.dampingRatio = 5.f;
	testJointDefinition(&jointDefinition);
}

BOOST_AUTO_TEST_CASE(NotHandledJoint, *boost::unit_test::tolerance(GetJointDefFixture::m_tolerance))
{
	b2RopeJointDef jointDefinition;
	jointDefinition.bodyA = bodyA;
	jointDefinition.bodyB = bodyB;
	jointDefinition.collideConnected = collideConnected;
	jointDefinition.userData = userData;
	testJointDefinition(&jointDefinition);
}

BOOST_AUTO_TEST_SUITE_END()
