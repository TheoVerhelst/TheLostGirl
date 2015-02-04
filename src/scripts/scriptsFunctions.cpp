#include <TheLostGirl/scripts/scriptsFunctions.h>

entityx::Entity nearestFoe(entityx::Entity self)
{
	return self;
}

float distanceFrom(entityx::Entity self, entityx::Entity target)
{
	return 100;
}

int attack(entityx::Entity self, entityx::Entity target)
{
	return 0;
}

bool canMove(entityx::Entity self)
{
	return true;
}

int move(entityx::Entity self, int direction)
{
	return 0;
}

int stop(entityx::Entity self)
{
	return 0;
}

bool canJump(entityx::Entity self)
{
	return true;
}

int jump(entityx::Entity self)
{
	return 0;
}
