#include <entityx/entityx.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/Command.h>

Command::Command():
	targetIsSpecific(false)
{
}

Command::Command(const Command& other):
	action(other.action),
	targetIsSpecific(other.targetIsSpecific)
{
	if(targetIsSpecific)
		entity = other.entity;
	else
		category = other.category;
}
