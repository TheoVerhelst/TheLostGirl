#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <Box2D/Box2D.h>
#include <entityx/entityx.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Parameters.h>
#include <TheLostGirl/Context.h>
#include <TheLostGirl/functions.h>


void handleResize()
{
	const float iw{float(Context::window->getSize().x)};
	const float ih{float(Context::window->getSize().y)};
	float fh(ih), fw(iw);//If size is in a 16:9 ratio, it won't change.
	if(iw / 16.f < ih / 9.f) //Taller than a 16:9 ratio
		fh = iw * (9.0f / 16.0f);
	else if(iw / 16.f > ih / 9.f) //Larger than a 16:9 ratio
		fw = ih * (16.0f / 9.0f);
	const float scalex{fw / iw}, scaley{fh / ih};
	sf::View view{Context::window->getView()};
	view.setViewport({(1 - scalex) / 2.0f, (1 - scaley) / 2.0f, scalex, scaley});
	view.setSize(1920.f/2, 1080.f/2);
	if(Context::parameters->bloomEnabled)
		Context::postEffectsTexture->setView(view);
	else
		Context::window->setView(view);
	view.setCenter({960.f/2, 540.f/2});
	Context::gui->setView(view);
}

sf::Color fadingColor(sf::Time dt, sf::Time fadingLength, bool in)
{
	float alpha{cap((dt / fadingLength) * 255.f, 0.f, 255.f)};
	alpha = in ? alpha : 255 - alpha;//Invert the fading if in is false, so if it is a fade out
	return sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha));
}

bool hasWhiteSpace(const std::string str)
{
	return std::any_of(str.cbegin(), str.cend(), [](const char& chr)
	{
		return isspace(chr);
	});
}

sf::Vector2f b2tosf(const b2Vec2& vec)
{
	return {vec.x, vec.y};
}

b2Vec2 sftob2(const sf::Vector2f& vec)
{
	return {vec.x, vec.y};
}

sf::Color b2ColorToSf(const b2Color& color)
{
	return sf::Color(static_cast<sf::Uint8>(color.r*255.f), static_cast<sf::Uint8>(color.g*255.f),
					 static_cast<sf::Uint8>(color.b*255.f), static_cast<sf::Uint8>(color.a*255.f));
}

bool isPlayer(entityx::Entity entity)
{
	assert(entity.valid());
	CategoryComponent::Handle categoryComponent(entity.component<CategoryComponent>());
	return categoryComponent and categoryComponent->category.test(Category::Player);
}

b2BodyDef getBodyDef(b2Body* body)
{
	b2BodyDef bodyDef;
	bodyDef.type = body->GetType();
	bodyDef.position = body->GetPosition();
	bodyDef.angle = body->GetAngle();
	bodyDef.linearVelocity = body->GetLinearVelocity();
	bodyDef.angularVelocity = body->GetAngularVelocity();
	bodyDef.linearDamping = body->GetLinearDamping();
	bodyDef.angularDamping = body->GetAngularDamping();
	bodyDef.allowSleep = body->IsSleepingAllowed();
	bodyDef.awake = body->IsAwake();
	bodyDef.fixedRotation = body->IsFixedRotation();
	bodyDef.bullet = body->IsBullet();
	bodyDef.active = body->IsActive();
	bodyDef.userData = body->GetUserData();
	bodyDef.gravityScale = body->GetGravityScale();
	return bodyDef;
}

b2FixtureDef getFixtureDef(b2Fixture* fixture)
{
	b2FixtureDef fixtureDef;
	fixtureDef.shape = fixture->GetShape();
	fixtureDef.userData = fixture->GetUserData();
	fixtureDef.friction = fixture->GetFriction();
	fixtureDef.restitution = fixture->GetRestitution();
	fixtureDef.density = fixture->GetDensity();
	fixtureDef.isSensor = fixture->IsSensor();
	fixtureDef.filter = fixture->GetFilterData();
	return fixtureDef;
}

b2JointDef* getJointDef(b2Joint* joint)
{
	b2JointDef* jointDef;
	switch(joint->GetType())
	{
		case e_revoluteJoint:
		{
			jointDef = new b2RevoluteJointDef;
			auto castedJointDef = static_cast<b2RevoluteJointDef*>(jointDef);
			auto castedJoint = static_cast<b2RevoluteJoint*>(joint);
			castedJointDef->localAnchorA = castedJoint->GetLocalAnchorA();
			castedJointDef->localAnchorB = castedJoint->GetLocalAnchorB();
			castedJointDef->referenceAngle = castedJoint->GetReferenceAngle();
			castedJointDef->enableLimit = castedJoint->IsLimitEnabled();
			castedJointDef->lowerAngle = castedJoint->GetReferenceAngle();
			castedJointDef->upperAngle = castedJoint->GetReferenceAngle();
			castedJointDef->enableMotor = castedJoint->IsMotorEnabled();
			castedJointDef->motorSpeed = castedJoint->GetMotorSpeed();
			castedJointDef->maxMotorTorque = castedJoint->GetMaxMotorTorque();
            break;
		}
		case e_prismaticJoint:
		{
			jointDef = new b2PrismaticJointDef;
			auto castedJointDef = static_cast<b2PrismaticJointDef*>(jointDef);
			auto castedJoint = static_cast<b2PrismaticJoint*>(joint);
			castedJointDef->localAnchorA = castedJoint->GetLocalAnchorA();
			castedJointDef->localAnchorB = castedJoint->GetLocalAnchorB();
			castedJointDef->localAxisA = castedJoint->GetLocalAxisA();
			castedJointDef->referenceAngle = castedJoint->GetReferenceAngle();
			castedJointDef->lowerTranslation = castedJoint->GetLowerLimit();
			castedJointDef->upperTranslation = castedJoint->GetUpperLimit();
			castedJointDef->enableLimit = castedJoint->IsLimitEnabled();
			castedJointDef->maxMotorForce = castedJoint->GetMaxMotorForce();
			castedJointDef->motorSpeed = castedJoint->GetMotorSpeed();
			castedJointDef->enableMotor = castedJoint->IsMotorEnabled();
            break;
		}
		case e_weldJoint:
		{
			jointDef = new b2WeldJointDef;
			auto castedJointDef = static_cast<b2WeldJointDef*>(jointDef);
			auto castedJoint = static_cast<b2WeldJoint*>(joint);
			castedJointDef->localAnchorA = castedJoint->GetLocalAnchorA();
			castedJointDef->localAnchorB = castedJoint->GetLocalAnchorB();
			castedJointDef->referenceAngle = castedJoint->GetReferenceAngle();
			castedJointDef->frequencyHz = castedJoint->GetFrequency();
			castedJointDef->dampingRatio = castedJoint->GetDampingRatio();
            break;
		}
		default:
			return nullptr;
	}
	jointDef->bodyA = joint->GetBodyA();
	jointDef->bodyB = joint->GetBodyB();
	jointDef->collideConnected = true;
	jointDef->userData = joint->GetUserData();
	return jointDef;
}

Direction operator!(const Direction& direction)
{
	if(direction == Direction::None)
		return Direction::None;
	else
	{
		const int max{static_cast<int>(Direction::None)};
		return Direction((static_cast<int>(direction) + (max / 2 )) % max);
	}
}

std::ostream& operator<<(std::ostream& ostream, const Direction& direction)
{
	switch(direction)
	{
		case Direction::Left:
			ostream << "direction::left";
			break;
		case Direction::Right:
			ostream << "direction::right";
			break;
		case Direction::Up:
			ostream << "direction::up";
			break;
		case Direction::Down:
			ostream << "direction::down";
			break;
		case Direction::None:
			ostream << "direction::none";
			break;
	}
	return ostream;
}

bool printError(const std::string& expression, const std::string& filename, unsigned int line)
{
	std::cerr << "Test failed in " << filename << " at line " << line << " : \"" << expression << "\"" << std::endl;
	return false;
}
