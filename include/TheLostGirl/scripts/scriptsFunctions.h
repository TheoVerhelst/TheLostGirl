#ifndef SCRIPTSFUNCTIONS_H
#define SCRIPTSFUNCTIONS_H

#include <iostream>

#include <entityx/Entity.h>

//Foe handling
entityx::Entity nearestFoe(entityx::Entity self);
float distanceFrom(entityx::Entity self, entityx::Entity target);
int attack(entityx::Entity self, entityx::Entity target);

//Moving
bool canMove(entityx::Entity self);
int move(entityx::Entity self, int direction);
int stop(entityx::Entity self);
bool canJump(entityx::Entity self);
int jump(entityx::Entity self);

//Debugging
template <typename T>
int print(T val)
{
	if(typeid(val) == typeid(bool))
		std::cout << (val ? "True\n" : "False\n");
	else
		std::cout << val << "\n";
	return 0;
}

#endif// SCRIPTSFUNCTIONS_H
