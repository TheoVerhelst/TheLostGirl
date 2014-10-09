#ifndef INTROSTATE_H
#define INTROSTATE_H

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class State;
class Context;
class StateStack;

class IntroState : public State
{
	public:
		IntroState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);
};

#endif // INTROSTATE_H
