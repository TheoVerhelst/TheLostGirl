#include <TheLostGirl/State.h>

void State::requestStackPop()
{
	Context::stateStack->popState();
}

void State::requestStateClear()
{
	Context::stateStack->clearStates();
}
