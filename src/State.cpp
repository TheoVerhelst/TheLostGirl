#include <TheLostGirl/State.hpp>

void State::requestStackPop()
{
	Context::stateStack->popState();
}

void State::requestStateClear()
{
	Context::stateStack->clearStates();
}
