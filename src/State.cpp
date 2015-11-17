#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include <Box2D/Dynamics/b2World.h>
#include <TheLostGirl/ResourceManager.h>
#include <TheLostGirl/Player.h>
#include <TheLostGirl/Command.h>
#include <TheLostGirl/State.h>

using entityx::EventManager;
using entityx::EntityManager;
using entityx::SystemManager;

void State::requestStackPop()
{
	Context::stateStack->popState();
}

void State::requestStateClear()
{
	Context::stateStack->clearStates();
}
