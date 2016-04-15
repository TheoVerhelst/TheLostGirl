#include <TheLostGirl/FlagSet.hpp>
#include <TheLostGirl/FixtureRoles.hpp>
#include <TheLostGirl/DestructionListener.hpp>

void DestructionListener::SayGoodbye(b2Joint*)
{
}

void DestructionListener::SayGoodbye(b2Fixture* fixture)
{
	delete static_cast<FlagSet<FixtureRole>*>(fixture->GetUserData());
}
