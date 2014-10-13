#ifndef GAMESTATE_H
#define GAMESTATE_H

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

class GameState : public State
{
	public:
		GameState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);

	private:
		entityx::Entity m_archer;
};

#endif // GAMESTATE_H
