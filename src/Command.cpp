#include <entityx/entityx.h>

#include <TheLostGirl/Category.h>

#include <TheLostGirl/Command.h>

Command::Command():
	action(),
	category(Category::None)
{
}
