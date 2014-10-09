#ifndef LOADINGSTATE_H
#define LOADINGSTATE_H

//Forward declarations
namespace sf
{
	class Event;
	class Time;
}
class State;
class Context;
class StateStack;

class LoadingState : public State
{
	public:
		LoadingState(StateStack& stack, Context context);
		virtual void draw();
		virtual bool update(sf::Time dt);
		virtual bool handleEvent(const sf::Event& event);

	private:
};

#endif // LOADINGSTATE_H
