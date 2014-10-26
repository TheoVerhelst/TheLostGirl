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
	m_archer(getContext().entityManager.create()),
	m_archerSprite(getContext().textureManager.get(Textures::Archer)),
	m_archerAnimations(),
	m_timeSystem()
{
	m_archer.assign<Controller>(true);
	m_archer.assign<Walk>(5.f);
	m_archer.assign<Jump>(5.f);
	m_archer.assign<DirectionComponent>(Direction::Right);
	m_archer.assign<CategoryComponent>(Category::Player|Category::CanFall);
	m_archer.assign<FallComponent>();
	//Sprite
	m_archer.assign<SpriteComponent>(&m_archerSprite);
	m_archerSprite.setTextureRect(sf::IntRect(0, 0, 100*scale, 200*scale));
	//Animations
	m_archer.assign<AnimationsComponent>(&m_archerAnimations);
	SpriteSheetAnimation stayLeft;
	stayLeft.addFrame(sf::IntRect(0, 0, 100*scale, 200*scale), 1.f);
	m_archerAnimations.addAnimation("stayLeft", stayLeft, 0, sf::seconds(1.f), false);
	
	SpriteSheetAnimation stayRight;
	stayRight.addFrame(sf::IntRect(0, 400*scale, 100*scale, 200*scale), 1.f);
	m_archerAnimations.addAnimation("stayRight", stayRight, 0, sf::seconds(1.f), false);
	
	SpriteSheetAnimation leftAnimation;
	leftAnimation.addFrame(sf::IntRect(100*1*scale, 0, 100*scale, 200*scale), 0.125f);
	leftAnimation.addFrame(sf::IntRect(100*2*scale, 0, 100*scale, 200*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*3*scale, 0, 100*scale, 200*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*4*scale, 0, 100*scale, 200*scale), 0.25f);
	leftAnimation.addFrame(sf::IntRect(100*5*scale, 0, 100*scale, 200*scale), 0.125f);
	m_archerAnimations.addAnimation("moveLeft", leftAnimation, 1, sf::seconds(.3f), true);
	
	SpriteSheetAnimation rightAnimation;
	rightAnimation.addFrame(sf::IntRect(100*1*scale, 400*scale, 100*scale, 200*scale), 0.125f);
	rightAnimation.addFrame(sf::IntRect(100*2*scale, 400*scale, 100*scale, 200*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*3*scale, 400*scale, 100*scale, 200*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*4*scale, 400*scale, 100*scale, 200*scale), 0.25f);
	rightAnimation.addFrame(sf::IntRect(100*5*scale, 400*scale, 100*scale, 200*scale), 0.125f);
	m_archerAnimations.addAnimation("moveRight", rightAnimation, 1, sf::seconds(.3f), true);
	
	SpriteSheetAnimation jumpLeftAnimation;
	jumpLeftAnimation.addFrame(sf::IntRect(100*0*scale, 600*scale, 100*scale, 200*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*1*scale, 600*scale, 100*scale, 200*scale), 0.33f);
	jumpLeftAnimation.addFrame(sf::IntRect(100*2*scale, 600*scale, 100*scale, 200*scale), 0.33f);
	m_archerAnimations.addAnimation("jumpLeft", jumpLeftAnimation, 2, sf::seconds(.3f), false);
	
	SpriteSheetAnimation jumpRightAnimation;
	jumpRightAnimation.addFrame(sf::IntRect(100*0*scale, 200*scale, 100*scale, 200*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*1*scale, 200*scale, 100*scale, 200*scale), 0.33f);
	jumpRightAnimation.addFrame(sf::IntRect(100*2*scale, 200*scale, 100*scale, 200*scale), 0.33f);
	m_archerAnimations.addAnimation("jumpRight", jumpRightAnimation, 2, sf::seconds(.3f), false);
	
	SpriteSheetAnimation fallLeft;
	fallLeft.addFrame(sf::IntRect(300*scale, 600*scale, 100*scale, 200*scale), 1.f);
	m_archerAnimations.addAnimation("fallLeft", fallLeft, 3, sf::seconds(1.f), false);
	
	SpriteSheetAnimation fallRight;
	fallRight.addFrame(sf::IntRect(300*scale, 200*scale, 100*scale, 200*scale), 1.f);
	m_archerAnimations.addAnimation("fallRight", fallRight, 3, sf::seconds(1.f), false);
	
	//Body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = {0, worldFrameSize.y/2};
	b2Body* body = getContext().world.CreateBody(&bodyDef);
	body->SetUserData(&m_archer);
	m_archer.assign<Body>(body);
	
	b2PolygonShape archerBox;
	archerBox.SetAsBox(100*scale/pixelScale*2, 200*scale/pixelScale*2);
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &archerBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.f;
	fixtureDef.restitution = 0.f;
	body->CreateFixture(&fixtureDef);
}

void GameState::draw()
{
	getContext().systemManager.update<Render>(sf::Time::Zero.asSeconds());
}

bool GameState::update(sf::Time elapsedTime)
{
	getContext().systemManager.update<Physics>(elapsedTime.asSeconds());
	getContext().systemManager.update<Actions>(elapsedTime.asSeconds());
	getContext().systemManager.update<AnimationSystem>(elapsedTime.asSeconds());
	getContext().systemManager.update<JumpSystem>(elapsedTime.asSeconds());
	m_timeSystem.update(elapsedTime);
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	switch(event.type)
	{
		case sf::Event::Closed:
			getContext().world.DestroyBody(m_archer.component<Body>()->body);
			getContext().world.ClearForces();
			m_archer.destroy();
			requestStackPop();
			break;

		case sf::Event::KeyPressed:
			if(event.key.code == sf::Keyboard::Escape)
			{
				getContext().world.DestroyBody(m_archer.component<Body>()->body);
				getContext().world.ClearForces();
				m_archer.destroy();
				requestStackPop();
				requestStackPush(States::MainMenu);
			}
			break;

		default:
			break;
	}
	getContext().player.handleEvent(event, getContext().commandQueue);
	return false;
}