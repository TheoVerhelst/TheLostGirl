#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <entityx/entityx.h>
#include <Box2D/Box2D.h>

#include <TheLostGirl/State.h>
#include <TheLostGirl/functions.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/ResourceManager.h>

class GameState : public State
{
	public:
		GameState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);

	private:
		Context& m_context;
		Player m_player;
		CommandQueue m_commandQueue;
		entityx::Entity m_archer;
};

#endif // GAMESTATE_H
