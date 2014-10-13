#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/components.h>
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
	m_archer(getContext().entityManager.create())
{
	//Sprite
	sf::Texture& archerTexture = getContext().textureManager.get(Textures::Archer);
	m_archer.assign<SpriteComponent>(new sf::Sprite(archerTexture));
	
	//Animation
	Animations::Handle animationsComponent = m_archer.assign<Animations>();
	SpriteSheetAnimation spritesheet;
	spritesheet.addFrame(sf::IntRect(53*0, 107, 53, 107), 0.125f);
	spritesheet.addFrame(sf::IntRect(53*1, 107, 53, 107), 0.125f);
	spritesheet.addFrame(sf::IntRect(53*2, 107, 53, 107), 0.125f);
	spritesheet.addFrame(sf::IntRect(53*3, 107, 53, 107), 0.125f);
	spritesheet.addFrame(sf::IntRect(53*4, 000, 53, 107), 0.100f);
	Animations::TimeAnimation timeAnime(spritesheet, sf::seconds(.8f));
	animationsComponent->animations.emplace("run", timeAnime);
	animationsComponent->isPlaying = true;
	animationsComponent->loops = true;
	animationsComponent->currentAnimation = "run";
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = {0, 4};
	b2Body* body = getContext().world.CreateBody(&bodyDef);
	m_archer.assign<Body>(body);

	b2PolygonShape archerBox;
	archerBox.SetAsBox(53.f/pixelScale*2, 107.f/pixelScale*2);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &archerBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.001f;
	fixtureDef.restitution = 0.98f;
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
	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	switch(event.type)
	{
		case sf::Event::Closed:
			m_archer.destroy();
			requestStackPop();
			break;

		case sf::Event::KeyPressed:
			if(event.key.code == sf::Keyboard::Escape)
			{
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