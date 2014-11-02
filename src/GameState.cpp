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
	m_groundEntity(getContext().entityManager.create()),
	m_fallingListener(),
	m_archer(getContext().entityManager.create()),
	m_archerSprite(getContext().textureManager.get(Textures::Archer)),
	m_archerAnimations(),
	m_arms(getContext().entityManager.create()),
	m_armsSprite(getContext().textureManager.get(Textures::Arms)),
	m_armsAnimations(),
	m_bow(getContext().entityManager.create()),
	m_bowSprite(getContext().textureManager.get(Textures::Bow)),
	m_bowAnimations(),
	m_timeSystem()
{
	initWorld();
	getContext().player.handleInitialInputState(getContext().commandQueue);
}

GameState::~GameState()
{
	getContext().world.DestroyBody(m_archer.component<Body>()->body);
	getContext().world.DestroyBody(m_groundEntity.component<Body>()->body);
	getContext().world.DestroyBody(m_arms.component<Body>()->body);
	getContext().world.DestroyBody(m_bow.component<Body>()->body);
	getContext().world.ClearForces();
	m_archer.destroy();
	m_arms.destroy();
	m_bow.destroy();
	m_groundEntity.destroy();
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
	
	b2BodyDef bd;
	b2Body* ground = getContext().world.CreateBody(&bd);
	b2EdgeShape shape;
	shape.Set(b2Vec2(-40.0f, screenSize.y), b2Vec2(40.0f, screenSize.y));
	ground->CreateFixture(&shape, 0.0f);
	ground->SetUserData(&m_groundEntity);
	m_entities.push_back(getContext().entityManager.create());
	m_entities[0].assign<Body>(ground);
	m_entities[0].assign<CategoryComponent>(Category::Ground);
//	m_groundEntity.assign<Body>(ground);
//	m_groundEntity.assign<CategoryComponent>(Category::Ground);
	
	getContext().world.SetContactListener(&m_fallingListener);
	
	//Archer initialization
	m_entities.push_back(getContext().entityManager.create());
//	m_archer.assign<Walk>(5.f);
//	m_archer.assign<Jump>(8.f);
//	m_archer.assign<DirectionComponent>(Direction::Right);
//	m_archer.assign<CategoryComponent>(Category::Player|Category::CanFall);
//	m_archer.assign<FallComponent>();
//	m_archer.assign<SpriteComponent>(&m_archerSprite);
//	m_archerSprite.setTextureRect(sf::IntRect(0, 0, 100.f*scale, 200.f*scale));
//	m_archer.assign<AnimationsComponent>(&m_archerAnimations);
	m_entities[1].assign<Walk>(5.f);
	m_entities[1].assign<Jump>(8.f);
	m_entities[1].assign<DirectionComponent>(Direction::Right);
	m_entities[1].assign<CategoryComponent>(Category::Player|Category::CanFall);
	m_entities[1].assign<FallComponent>();
	m_sprites.push_back(sf::Sprite(getContext().textureManager.get(Textures::Archer)));
	m_entities[1].assign<SpriteComponent>(&m_sprites[0]);
	m_sprites[0].setTextureRect(sf::IntRect(0, 0, 100.f*scale, 200.f*scale));
	m_animations.push_back(Animations());
	m_entities[1].assign<AnimationsComponent>(&m_animations[0]);

	SpriteSheetAnimation stayLeft;
	stayLeft.addFrame(sf::IntRect(0, 0, 100.f*scale, 200.f*scale), 1.f);
	m_animations[0].addAnimation("stayLeft", stayLeft, 0, sf::seconds(1.f), false);

	SpriteSheetAnimation stayRight;
	stayRight.addFrame(sf::IntRect(0, 400.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations[0].addAnimation("stayRight", stayRight, 0, sf::seconds(1.f), false);
	m_animations[0].play("stayRight");//The character is diriged to right

	SpriteSheetAnimation leftAnimation;
	leftAnimation.addFrame(sf::IntRect(100*1.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	leftAnimation.addFrame(sf::IntRect(100*2.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*3.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*4.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*5.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	m_animations[0].addAnimation("moveLeft", leftAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation rightAnimation;
	rightAnimation.addFrame(sf::IntRect(100*1.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	rightAnimation.addFrame(sf::IntRect(100*2.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*3.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*4.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*5.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	m_animations[0].addAnimation("moveRight", rightAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation jumpLeftAnimation;
	jumpLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_animations[0].addAnimation("jumpLeft", jumpLeftAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation jumpRightAnimation;
	jumpRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_animations[0].addAnimation("jumpRight", jumpRightAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation fallLeft;
	fallLeft.addFrame(sf::IntRect(300.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations[0].addAnimation("fallLeft", fallLeft, 3, sf::seconds(1.f), false);

	SpriteSheetAnimation fallRight;
	fallRight.addFrame(sf::IntRect(300.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_animations[0].addAnimation("fallRight", fallRight, 3, sf::seconds(1.f), false);

	b2BodyDef archerBodyDef;
	archerBodyDef.type = b2_dynamicBody;
	archerBodyDef.position = {0, 0};
	archerBodyDef.fixedRotation = true;
	archerBodyDef.userData = &m_entities[1];
	b2Body* archerBody = getContext().world.CreateBody(&archerBodyDef);
	m_entities[1].assign<Body>(archerBody);

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
	m_arms.assign<DirectionComponent>(Direction::Right);
	m_arms.assign<CategoryComponent>(Category::Player);
	m_arms.assign<BendComponent>(400.f);
	m_arms.assign<SpriteComponent>(&m_armsSprite);
	m_arms.assign<AnimationsComponent>(&m_armsAnimations);

	SpriteSheetAnimation bendLeftAnimation;
	bendLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*3.f*scale, 250.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	m_armsAnimations.addAnimation("bendLeft", bendLeftAnimation);

	SpriteSheetAnimation bendRightAnimation;
	bendRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*3.f*scale, 50.f*scale, 100.f*scale, 70.f*scale), 0.25f);
	m_armsAnimations.addAnimation("bendRight", bendRightAnimation);
	m_armsAnimations.activate("bendRight");

	b2BodyDef armBodyDef;
	armBodyDef.type = b2_dynamicBody;
	armBodyDef.position = {0, 50.f*scale/pixelScale};
	armBodyDef.userData = &m_arms;
	b2Body* armBody = getContext().world.CreateBody(&armBodyDef);
	m_arms.assign<Body>(armBody);

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

	//Arms initiailization
	m_bow.assign<DirectionComponent>(Direction::Right);
	m_bow.assign<CategoryComponent>(Category::Player);
	m_bow.assign<SpriteComponent>(&m_bowSprite);
	m_bow.assign<BendComponent>(400.f);
	m_bow.assign<AnimationsComponent>(&m_bowAnimations);

	SpriteSheetAnimation bowBendLeftAnimation;
	bowBendLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendLeftAnimation.addFrame(sf::IntRect(100*3.f*scale, 245.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	m_bowAnimations.addAnimation("bendLeft", bowBendLeftAnimation);

	SpriteSheetAnimation bowBendRightAnimation;
	bowBendRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	bowBendRightAnimation.addFrame(sf::IntRect(100*3.f*scale, 45.f*scale, 100.f*scale, 100.f*scale), 0.25f);
	m_bowAnimations.addAnimation("bendRight", bowBendRightAnimation);
	m_bowAnimations.activate("bendRight");

	b2BodyDef bowBodyDef;
	bowBodyDef.type = b2_dynamicBody;
	bowBodyDef.position = {0, 50.f*scale/pixelScale};
	bowBodyDef.userData = &m_bow;
	b2Body* bowBody = getContext().world.CreateBody(&bowBodyDef);
	m_bow.assign<Body>(bowBody);

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