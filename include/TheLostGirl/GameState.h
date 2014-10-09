#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <queue>

#include <TheLostGirl/Player.h>

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
namespace entityx
{
	class Entity;
}
class State;
class Context;
class StateStack;
class Command;

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
		std::queue<Command> m_commandQueue;
		entityx::Entity m_archer;
};

#endif // GAMESTATE_H
