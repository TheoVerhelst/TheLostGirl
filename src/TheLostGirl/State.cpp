#include <TheLostGirl/State.hpp>

void State::requestStackPop()
{
	Context::getStateStack().popState();
}

void State::requestStateClear()
{
	Context::getStateStack().clearStates();
}
