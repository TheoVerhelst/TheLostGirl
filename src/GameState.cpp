#include <iostream>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>
#include <dist/json/json.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>
#include <TheLostGirl/Parameters.h>

#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_entities(),
	m_sprites(),
	m_animations(),
	m_fallingListener(),
	m_timeSystem()
{
	initWorld();
	getContext().player.handleInitialInputState(getContext().commandQueue);
}

GameState::~GameState()
{
	for(auto& entity : m_entities)
	{
		getContext().world.DestroyBody(entity.component<Body>()->body);
		entity.destroy();
	}
	getContext().world.ClearForces();
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
	//The drag and drop system draw a line on the screen, so we must put it here
	getContext().systemManager.update<DragAndDropSystem>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<FallSystem>(elapsedTime.asSeconds());
	m_timeSystem.update(elapsedTime);
	return false;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);
	
	getContext().player.handleEvent(event, getContext().commandQueue);
	//Update the drag and drop state
	bool isDragAndDropActive{getContext().player.isActived(Player::Action::Bend)};
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(isDragAndDropActive);

	return false;
}

void GameState::initWorld()
{
	
	float scale = getContext().parameters.scale;
	float pixelScale = getContext().parameters.pixelScale;
	sf::Vector2f screenSize = getContext().parameters.worldFrameSize;
	
	m_entities.push_front(getContext().entityManager.create());
	b2BodyDef bd;
	b2Body* ground = getContext().world.CreateBody(&bd);
	b2EdgeShape shape;
	shape.Set(b2Vec2(-40.0f, screenSize.y), b2Vec2(40.0f, screenSize.y));
	ground->CreateFixture(&shape, 0.0f);
	ground->SetUserData(&m_entities.front());
	m_entities.front().assign<Body>(ground);
	m_entities.front().assign<CategoryComponent>(Category::Ground);
	
	getContext().world.SetContactListener(&m_fallingListener);
	
	//Archer initialization
	m_entities.push_front(getContext().entityManager.create());
	m_entities.front().assign<Walk>(5.f);
	m_entities.front().assign<Jump>(8.f);
	m_entities.front().assign<DirectionComponent>(Direction::Right);
	m_entities.front().assign<CategoryComponent>(Category::Player|Category::CanFall);
	m_entities.front().assign<FallComponent>();
	m_sprites.push_front(sf::Sprite(getContext().textureManager.get(Textures::Archer)));
	m_entities.front().assign<SpriteComponent>(&m_sprites.front());
	m_sprites.front().setTextureRect(sf::IntRect(0, 0, 100.f*scale, 200.f*scale));
	m_animations.push_front(Animations());
	m_entities.front().assign<AnimationsComponent>(&m_animations.front());

	SpriteSheetAnimation stayLeft;
	stayLeft.addFrame(sf::IntRect(0, 0, 100.f*scale, 200.f*scale), 1.f);
	m_animations.front().addAnimation("stayLeft", stayLeft, 0, sf::seconds(1.f), false);

	SpriteSheetAnimation stayRight;
	stayRight.addFrame(sf::IntRect(0, 400.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations.front().addAnimation("stayRight", stayRight, 0, sf::seconds(1.f), false);
	m_animations.front().play("stayRight");//The character is diriged to right

	SpriteSheetAnimation leftAnimation;
	leftAnimation.addFrame(sf::IntRect(100*1.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	leftAnimation.addFrame(sf::IntRect(100*2.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*3.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*4.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*5.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	m_animations.front().addAnimation("moveLeft", leftAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation rightAnimation;
	rightAnimation.addFrame(sf::IntRect(100*1.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	rightAnimation.addFrame(sf::IntRect(100*2.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*3.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*4.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*5.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	m_animations.front().addAnimation("moveRight", rightAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation jumpLeftAnimation;
	jumpLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_animations.front().addAnimation("jumpLeft", jumpLeftAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation jumpRightAnimation;
	jumpRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_animations.front().addAnimation("jumpRight", jumpRightAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation fallLeft;
	fallLeft.addFrame(sf::IntRect(300.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations.front().addAnimation("fallLeft", fallLeft, 3, sf::seconds(1.f), false);

	SpriteSheetAnimation fallRight;
	fallRight.addFrame(sf::IntRect(300.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations.front().addAnimation("fallRight", fallRight, 3, sf::seconds(1.f), false);

	b2BodyDef archerBodyDef;
	archerBodyDef.type = b2_dynamicBody;
	archerBodyDef.position = {0, 0};
	archerBodyDef.fixedRotation = true;
	archerBodyDef.userData = &m_entities.front();
	b2Body* archerBody = getContext().world.CreateBody(&archerBodyDef);
	m_entities.front().assign<Body>(archerBody);

	b2PolygonShape archerBox;
	archerBox.SetAsBox((100.f*scale/pixelScale)/2, (200.f*scale/pixelScale)/2, {(100.f*scale/pixelScale)/2, (200.f*scale/pixelScale)/2}, 0);

	b2FixtureDef archerFixtureDef;
	archerFixtureDef.shape = &archerBox;
	archerFixtureDef.density = 1.0f;
	archerFixtureDef.friction = 1.f;
	archerFixtureDef.restitution = 0.f;
	archerFixtureDef.filter.groupIndex = -1;
	archerBody->CreateFixture(&archerFixtureDef);

	//Arms initiailization
	m_entities.push_front(getContext().entityManager.create());
	m_entities.front().assign<DirectionComponent>(Direction::Right);
	m_entities.front().assign<CategoryComponent>(Category::Player);
	m_entities.front().assign<BendComponent>(400.f);
	m_sprites.push_front(sf::Sprite(getContext().textureManager.get(Textures::Arms)));
	m_entities.front().assign<SpriteComponent>(&m_sprites.front());
	m_animations.push_front(Animations());
	m_entities.front().assign<AnimationsComponent>(&m_animations.front());
	
	SpriteSheetAnimation bendLeftAnimation;
	bendLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*3.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	m_animations.front().addAnimation("bendLeft", bendLeftAnimation);
	
	SpriteSheetAnimation bendRightAnimation;
	bendRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*3.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	m_animations.front().addAnimation("bendRight", bendRightAnimation);
	m_animations.front().activate("bendRight");
	
	b2BodyDef armBodyDef;
	armBodyDef.type = b2_dynamicBody;
	armBodyDef.position = {0, 50.f*scale/pixelScale};
	armBodyDef.userData = &m_entities.front();
	b2Body* armBody = getContext().world.CreateBody(&armBodyDef);
	m_entities.front().assign<Body>(armBody);

	b2PolygonShape armBox;
	armBox.SetAsBox((100.f*scale/pixelScale)/2, (70.f*scale/pixelScale)/2, {(100.f*scale/pixelScale)/2, (70.f*scale/pixelScale)/2}, 0);

	b2FixtureDef armFixtureDef;
	armFixtureDef.shape = &armBox;
	armFixtureDef.density = 1.f;
	armFixtureDef.friction = 1.f;
	armFixtureDef.restitution = 0.f;
	armFixtureDef.filter.groupIndex = -1;
	armBody->CreateFixture(&armFixtureDef);
	
	//Joint between archer and his arms
	b2RevoluteJointDef jointDef;
	jointDef.Initialize(armBody, archerBody, archerBody->GetPosition() + b2Vec2(50.f*scale/pixelScale, 68.f*scale/pixelScale));
	jointDef.lowerAngle = -b2_pi/2.f;//-90 degrees, to the foots of the archer
	jointDef.upperAngle = b2_pi;//180 degrees, the horizontal axis in the back of the archer
	jointDef.enableLimit = true;
	jointDef.maxMotorTorque = 10.0f;
	jointDef.enableMotor = true;
	getContext().world.CreateJoint(&jointDef);

	//Bow initiailization
	m_entities.push_front(getContext().entityManager.create());
	m_entities.front().assign<DirectionComponent>(Direction::Right);
	m_entities.front().assign<CategoryComponent>(Category::Player);
	m_sprites.push_front(sf::Sprite(getContext().textureManager.get(Textures::Bow)));
	m_entities.front().assign<SpriteComponent>(&m_sprites.front());
	m_entities.front().assign<BendComponent>(400.f);
	m_animations.push_front(Animations());
	m_entities.front().assign<AnimationsComponent>(&m_animations.front());

	SpriteSheetAnimation bowBendLeftAnimation;
	bowBendLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*3.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	m_animations.front().addAnimation("bendLeft", bowBendLeftAnimation);

	SpriteSheetAnimation bowBendRightAnimation;
	bowBendRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*3.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	m_animations.front().addAnimation("bendRight", bowBendRightAnimation);
	m_animations.front().activate("bendRight");

	b2BodyDef bowBodyDef;
	bowBodyDef.type = b2_dynamicBody;
	bowBodyDef.position = {0, 50.f*scale/pixelScale};
	bowBodyDef.userData = &m_entities.front();
	b2Body* bowBody = getContext().world.CreateBody(&bowBodyDef);
	m_entities.front().assign<Body>(bowBody);

	b2PolygonShape bowBox;
	bowBox.SetAsBox((100.f*scale/pixelScale)/2, (100.f*scale/pixelScale)/2, {(100.f*scale/pixelScale)/2, (100.f*scale/pixelScale)/2}, 0);

	b2FixtureDef bowFixtureDef;
	bowFixtureDef.shape = &bowBox;
	bowFixtureDef.density = 1.f;
	bowFixtureDef.friction = 1.f;
	bowFixtureDef.restitution = 0.f;
	bowFixtureDef.filter.groupIndex = -1;
	bowBody->CreateFixture(&bowFixtureDef);
	
	//Joint between archer and his bow
	b2RevoluteJointDef secondJointDef;
	secondJointDef.Initialize(bowBody, archerBody, archerBody->GetPosition() + b2Vec2(50.f*scale/pixelScale, 68.f*scale/pixelScale));
	secondJointDef.lowerAngle = -b2_pi/2.f;//-90 degrees, to the foots of the archer
	secondJointDef.upperAngle = b2_pi;//180 degrees, the horizontal axis in the back of the archer
	secondJointDef.enableLimit = true;
	secondJointDef.maxMotorTorque = 10.0f;
	secondJointDef.enableMotor = true;
	getContext().world.CreateJoint(&secondJointDef);
}