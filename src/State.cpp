#include <TheLostGirl/State.hpp>

void State::requestStackPop()
{
	getStateStack().popState();
}

void State::requestStateClear()
{
	getStateStack().clearStates();
}
