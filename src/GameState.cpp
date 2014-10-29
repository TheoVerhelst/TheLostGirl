#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/StateStack.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/systems.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/ResourceIdentifiers.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/SpriteSheetAnimation.h>

#include <TheLostGirl/GameState.h>

GameState::GameState(StateStack& stack, Context context) :
	State(stack, context),
	m_groundEntity(getContext().entityManager.create()),
	m_fallingListener(),
	m_archer(getContext().entityManager.create()),
	m_archerSprite(getContext().textureManager.get(Textures::Archer)),
	m_archerAnimations(),
	m_arms(getContext().entityManager.create()),
	m_armsSprite(getContext().textureManager.get(Textures::Arms)),
	m_armsAnimations(),
	m_timeSystem()
{
	initWorld();

	//Archer initialization
	m_archer.assign<Walk>(5.f);
	m_archer.assign<Jump>(5.f);
	m_archer.assign<DirectionComponent>(Direction::Right);
	m_archer.assign<CategoryComponent>(Category::Player|Category::CanFall);
	m_archer.assign<FallComponent>();
	m_archer.assign<SpriteComponent>(&m_archerSprite);
	m_archerSprite.setTextureRect(sf::IntRect(0, 0, 100.f*scale, 200.f*scale));
	m_archer.assign<AnimationsComponent>(&m_archerAnimations);

	SpriteSheetAnimation stayLeft;
	stayLeft.addFrame(sf::IntRect(0, 0, 100.f*scale, 200.f*scale), 1.f);
	m_archerAnimations.addAnimation("stayLeft", stayLeft, 0, sf::seconds(1.f), false);

	SpriteSheetAnimation stayRight;
	stayRight.addFrame(sf::IntRect(0, 400.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_archerAnimations.addAnimation("stayRight", stayRight, 0, sf::seconds(1.f), false);
	m_archerAnimations.play("stayRight");//The character is diriged to right

	SpriteSheetAnimation leftAnimation;
	leftAnimation.addFrame(sf::IntRect(100*1.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	leftAnimation.addFrame(sf::IntRect(100*2.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*3.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*4.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*5.f*scale, 0, 100.f*scale, 200.f*scale), 0.125f);
	m_archerAnimations.addAnimation("moveLeft", leftAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation rightAnimation;
	rightAnimation.addFrame(sf::IntRect(100*1.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	rightAnimation.addFrame(sf::IntRect(100*2.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*3.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*4.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*5.f*scale, 400.f*scale, 100.f*scale, 200.f*scale), 0.125f);
	m_archerAnimations.addAnimation("moveRight", rightAnimation, 1, sf::seconds(.3f), true);

	SpriteSheetAnimation jumpLeftAnimation;
	jumpLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_archerAnimations.addAnimation("jumpLeft", jumpLeftAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation jumpRightAnimation;
	jumpRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.33f);
	m_archerAnimations.addAnimation("jumpRight", jumpRightAnimation, 2, sf::seconds(.3f), false);

	SpriteSheetAnimation fallLeft;
	fallLeft.addFrame(sf::IntRect(300.f*scale, 600.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_archerAnimations.addAnimation("fallLeft", fallLeft, 3, sf::seconds(1.f), false);

	SpriteSheetAnimation fallRight;
	fallRight.addFrame(sf::IntRect(300.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 1.f);
	m_archerAnimations.addAnimation("fallRight", fallRight, 3, sf::seconds(1.f), false);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = {0, 200.f*scale/pixelScale*2-0.1f};
	b2Body* body = getContext().world.CreateBody(&bodyDef);
	body->SetUserData(&m_archer);
	m_archer.assign<Body>(body);

	b2PolygonShape archerBox;
	archerBox.SetAsBox(100.f*scale/pixelScale*2, 200.f*scale/pixelScale*2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &archerBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.f;
	fixtureDef.restitution = 0.f;
	body->CreateFixture(&fixtureDef);

	//Arms initiailization
	m_arms.assign<DirectionComponent>(Direction::Right);
	m_arms.assign<CategoryComponent>(Category::Player);
	m_arms.assign<SpriteComponent>(&m_armsSprite);
	m_armsSprite.setTextureRect(sf::IntRect(0, 0, 100.f*scale, 200.f*scale));
	m_arms.assign<AnimationsComponent>(&m_armsAnimations);

	SpriteSheetAnimation bendLeftAnimation;
	bendLeftAnimation.addFrame(sf::IntRect(100*0.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*1.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*2.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	bendLeftAnimation.addFrame(sf::IntRect(100*3.f*scale, 200.f*scale, 100.f*scale, 200.f*scale), 0.25f);
	m_armsAnimations.addAnimation("bendLeft", bendLeftAnimation);

	SpriteSheetAnimation bendRightAnimation;
	bendRightAnimation.addFrame(sf::IntRect(100*0.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*1.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*2.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	bendRightAnimation.addFrame(sf::IntRect(100*3.f*scale, 0, 100.f*scale, 200.f*scale), 0.25f);
	m_armsAnimations.addAnimation("bendRight", bendRightAnimation);
	m_armsAnimations.activate("bendRight");
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
	getContext().systemManager.update<DragAndDropSystem>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<FallSystem>(elapsedTime.asSeconds());

	//Update the drag and drop state
	getContext().systemManager.system<DragAndDropSystem>()->setDragAndDropActivation(getContext().player.isDragAndDropActive());

	m_timeSystem.update(elapsedTime);
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::Closed or
		(event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape))
	{
			getContext().world.DestroyBody(m_archer.component<Body>()->body);
			getContext().world.DestroyBody(m_groundEntity.component<Body>()->body);
			getContext().world.ClearForces();
			m_archer.destroy();
			m_arms.destroy();
			m_groundEntity.destroy();
			requestStackPop();
			if(event.type == sf::Event::KeyPressed)
				requestStackPush(States::MainMenu);
	}
	getContext().player.handleEvent(event, getContext().commandQueue);
	return false;
}

void GameState::initWorld()
{
	b2BodyDef bd;
	b2Body* ground = getContext().world.CreateBody(&bd);

	b2EdgeShape shape;
	shape.Set(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
	ground->CreateFixture(&shape, 0.0f);
	ground->SetUserData(&m_groundEntity);

	m_groundEntity.assign<Body>(ground);
	m_groundEntity.assign<CategoryComponent>(Category::Ground);

	getContext().world.SetContactListener(&m_fallingListener);
}