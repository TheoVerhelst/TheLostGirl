#include <TheLostGirl/FlagSet.h>
#include <TheLostGirl/FixtureRoles.h>
#include <TheLostGirl/DestructionListener.h>

DestructionListener::~DestructionListener()
{
}

void DestructionListener::SayGoodbye(b2Joint*)
{
}

void DestructionListener::SayGoodbye(b2Fixture* fixture)
{
	delete static_cast<FlagSet<FixtureRole>*>(fixture->GetUserData());
}
